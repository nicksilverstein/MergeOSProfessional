/*
 * Copyright 2012 Jacek Caban for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define OEMRESOURCE

#include <assert.h>

#include "urlmon_main.h"
#include "resource.h"

#include "advpub.h"
#include "fdi.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(urlmon);

static const WCHAR ctxW[] = {'c','t','x',0};
static const WCHAR cab_extW[] = {'.','c','a','b',0};
static const WCHAR infW[] = {'i','n','f',0};
static const WCHAR dllW[] = {'d','l','l',0};
static const WCHAR ocxW[] = {'o','c','x',0};

enum install_type {
    INSTALL_UNKNOWN,
    INSTALL_DLL,
    INSTALL_INF
};

typedef struct {
    IUri *uri;
    IBindStatusCallback *callback;
    BOOL release_on_stop;
    BOOL cancel;
    WCHAR *install_file;
    const WCHAR *cache_file;
    const WCHAR *tmp_dir;
    const WCHAR *file_name;
    enum install_type install_type;
    HWND hwnd;
    int counter;
    INT_PTR timer;
} install_ctx_t;

static void release_install_ctx(install_ctx_t *ctx)
{
    if(ctx->uri)
        IUri_Release(ctx->uri);
    if(ctx->callback)
        IBindStatusCallback_Release(ctx->callback);
    heap_free(ctx->install_file);
    heap_free(ctx);
}

static inline BOOL file_exists(const WCHAR *file_name)
{
    return GetFileAttributesW(file_name) != INVALID_FILE_ATTRIBUTES;
}

#ifdef __MERGEOS__

/* The following definitions were copied from dll/win32/advpack32/files.c */

/* SESSION Operation */
#define EXTRACT_FILLFILELIST  0x00000001
#define EXTRACT_EXTRACTFILES  0x00000002

struct FILELIST{
    LPSTR FileName;
    struct FILELIST *next;
    BOOL DoExtract;
};

typedef struct {
    INT FileSize;
    ERF Error;
    struct FILELIST *FileList;
    INT FileCount;
    INT Operation;
    CHAR Destination[MAX_PATH];
    CHAR CurrentFile[MAX_PATH];
    CHAR Reserved[MAX_PATH];
    struct FILELIST *FilterList;
} SESSION;

static HRESULT (WINAPI *pExtract)(SESSION*, LPCSTR);


/* The following functions were copied from dll/win32/advpack32/files.c
   All unused arguments are removed */

static void free_file_node(struct FILELIST *pNode)
{
    HeapFree(GetProcessHeap(), 0, pNode->FileName);
    HeapFree(GetProcessHeap(), 0, pNode);
}

static void free_file_list(SESSION* session)
{
    struct FILELIST *next, *curr = session->FileList;

    while (curr)
    {
        next = curr->next;
        free_file_node(curr);
        curr = next;
    }
}

HRESULT WINAPI Modified_ExtractFilesA(LPCSTR CabName, LPCSTR ExpandDir)
{   
    SESSION session;
    HMODULE hCabinet;
    HRESULT res = S_OK;
    LPSTR szConvertedList = NULL;

    TRACE("(%s, %s)\n", debugstr_a(CabName), debugstr_a(ExpandDir));

    if (!CabName || !ExpandDir)
        return E_INVALIDARG;

    if (GetFileAttributesA(ExpandDir) == INVALID_FILE_ATTRIBUTES)
        return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);

    hCabinet = LoadLibraryA("cabinet.dll");
    if (!hCabinet)
        return E_FAIL;

    ZeroMemory(&session, sizeof(SESSION));

    pExtract = (void *)GetProcAddress(hCabinet, "Extract");
    if (!pExtract)
    {
        res = E_FAIL;
        goto done;
    }

    lstrcpyA(session.Destination, ExpandDir);

    session.Operation |= (EXTRACT_FILLFILELIST | EXTRACT_EXTRACTFILES);
    res = pExtract(&session, CabName);

done:
    free_file_list(&session);
    FreeLibrary(hCabinet);
    HeapFree(GetProcessHeap(), 0, szConvertedList);

    return res;
}



HRESULT WINAPI Modified_ExtractFilesW(LPCWSTR CabName, LPCWSTR ExpandDir)
{
    char *cab_name = NULL, *expand_dir = NULL;
    HRESULT hres = S_OK;

    TRACE("(%s, %s, %d)\n", debugstr_w(CabName), debugstr_w(ExpandDir));

    if(CabName) {
        cab_name = heap_strdupWtoA(CabName);
        if(!cab_name)
            return E_OUTOFMEMORY;
    }

    if(ExpandDir) {
        expand_dir = heap_strdupWtoA(ExpandDir);
        if(!expand_dir)
            hres = E_OUTOFMEMORY;
    }

    /* cabinet.dll, which does the real job of extracting files, doesn't have UNICODE API,
    so we need W->A conversion at some point anyway. */
    if(SUCCEEDED(hres))
        hres = Modified_ExtractFilesA(cab_name, expand_dir);

    heap_free(cab_name);
    heap_free(expand_dir);
    return hres;
}

#endif


static HRESULT extract_cab_file(install_ctx_t *ctx)
{
    size_t path_len, file_len;
    WCHAR *ptr;
    HRESULT hres;

#ifdef __MERGEOS__
    hres = Modified_ExtractFilesW(ctx->cache_file, ctx->tmp_dir);
#else
    hres = ExtractFilesW(ctx->cache_file, ctx->tmp_dir, 0, NULL, NULL, 0);
#endif
    if(FAILED(hres)) {
        WARN("ExtractFilesW failed: %08x\n", hres);
        return hres;
    }

    path_len = lstrlenW(ctx->tmp_dir);
    file_len = lstrlenW(ctx->file_name);
    ctx->install_file = heap_alloc((path_len+file_len+2)*sizeof(WCHAR));
    if(!ctx->install_file)
        return E_OUTOFMEMORY;

    memcpy(ctx->install_file, ctx->tmp_dir, path_len*sizeof(WCHAR));
    ctx->install_file[path_len] = '\\';
    memcpy(ctx->install_file+path_len+1, ctx->file_name, (file_len+1)*sizeof(WCHAR));

    /* NOTE: Assume that file_name contains ".cab" extension */
    ptr = ctx->install_file+path_len+1+file_len-3;

    memcpy(ptr, infW, sizeof(infW));
    if(file_exists(ctx->install_file)) {
        ctx->install_type = INSTALL_INF;
        return S_OK;
    }

    memcpy(ptr, dllW, sizeof(dllW));
    if(file_exists(ctx->install_file)) {
        ctx->install_type = INSTALL_DLL;
        return S_OK;
    }

    memcpy(ptr, ocxW, sizeof(ocxW));
    if(file_exists(ctx->install_file)) {
        ctx->install_type = INSTALL_DLL;
        return S_OK;
    }

    FIXME("No known install file\n");
    return E_NOTIMPL;
}

static HRESULT setup_dll(install_ctx_t *ctx)
{
    HMODULE module;
    HRESULT hres;

    HRESULT (WINAPI *reg_func)(void);

    module = LoadLibraryW(ctx->install_file);
    if(!module)
        return E_FAIL;

    reg_func = (void*)GetProcAddress(module, "DllRegisterServer");
    if(reg_func) {
        hres = reg_func();
    }else {
        WARN("no DllRegisterServer function\n");
        hres = E_FAIL;
    }

    FreeLibrary(module);
    return hres;
}

static void expand_command(install_ctx_t *ctx, const WCHAR *cmd, WCHAR *buf, size_t *size)
{
    const WCHAR *ptr = cmd, *prev_ptr = cmd;
    size_t len = 0, len2;

    static const WCHAR expand_dirW[] = {'%','E','X','T','R','A','C','T','_','D','I','R','%'};

    while((ptr = wcschr(ptr, '%'))) {
        if(buf)
            memcpy(buf+len, prev_ptr, ptr-prev_ptr);
        len += ptr-prev_ptr;

        if(!_wcsnicmp(ptr, expand_dirW, ARRAY_SIZE(expand_dirW))) {
            len2 = lstrlenW(ctx->tmp_dir);
            if(buf)
                memcpy(buf+len, ctx->tmp_dir, len2*sizeof(WCHAR));
            len += len2;
            ptr += ARRAY_SIZE(expand_dirW);
        }else {
            FIXME("Can't expand %s\n", debugstr_w(ptr));
            if(buf)
                buf[len] = '%';
            len++;
            ptr++;
        }

        prev_ptr = ptr;
    }

    if(buf)
        lstrcpyW(buf+len, prev_ptr);
    *size = len + lstrlenW(prev_ptr) + 1;
}

static HRESULT process_hook_section(install_ctx_t *ctx, const WCHAR *sect_name)
{
    WCHAR buf[2048], val[2*MAX_PATH];
    const WCHAR *key;
    DWORD len;
    HRESULT hres;

    static const WCHAR runW[] = {'r','u','n',0};

    len = GetPrivateProfileStringW(sect_name, NULL, NULL, buf, ARRAY_SIZE(buf), ctx->install_file);
    if(!len)
        return S_OK;

    for(key = buf; *key; key += lstrlenW(key)+1) {
        if(!wcsicmp(key, runW)) {
            WCHAR *cmd;
            size_t size;

            len = GetPrivateProfileStringW(sect_name, runW, NULL, val, ARRAY_SIZE(val), ctx->install_file);

            TRACE("Run %s\n", debugstr_w(val));

            expand_command(ctx, val, NULL, &size);

            cmd = heap_alloc(size*sizeof(WCHAR));
            if(!cmd)
                heap_free(cmd);

            expand_command(ctx, val, cmd, &size);
            hres = RunSetupCommandW(ctx->hwnd, cmd, NULL, ctx->tmp_dir, NULL, NULL, 0, NULL);
            heap_free(cmd);
            if(FAILED(hres))
                return hres;
        }else {
            FIXME("Unsupported hook %s\n", debugstr_w(key));
            return E_NOTIMPL;
        }
    }

    return S_OK;
}

static HRESULT install_inf_file(install_ctx_t *ctx)
{
    WCHAR buf[2048], sect_name[128];
    BOOL default_install = TRUE;
    const WCHAR *key;
    DWORD len;
    HRESULT hres;

    static const WCHAR setup_hooksW[] = {'S','e','t','u','p',' ','H','o','o','k','s',0};
    static const WCHAR add_codeW[] = {'A','d','d','.','C','o','d','e',0};

    len = GetPrivateProfileStringW(setup_hooksW, NULL, NULL, buf, ARRAY_SIZE(buf), ctx->install_file);
    if(len) {
        default_install = FALSE;

        for(key = buf; *key; key += lstrlenW(key)+1) {
            TRACE("[Setup Hooks] key: %s\n", debugstr_w(key));

            len = GetPrivateProfileStringW(setup_hooksW, key, NULL, sect_name, ARRAY_SIZE(sect_name),
                    ctx->install_file);
            if(!len) {
                WARN("Could not get key value\n");
                return E_FAIL;
            }

            hres = process_hook_section(ctx, sect_name);
            if(FAILED(hres))
                return hres;
        }
    }

    len = GetPrivateProfileStringW(add_codeW, NULL, NULL, buf, ARRAY_SIZE(buf), ctx->install_file);
    if(len) {
        default_install = FALSE;

        for(key = buf; *key; key += lstrlenW(key)+1) {
            TRACE("[Add.Code] key: %s\n", debugstr_w(key));

            len = GetPrivateProfileStringW(add_codeW, key, NULL, sect_name, ARRAY_SIZE(sect_name),
                    ctx->install_file);
            if(!len) {
                WARN("Could not get key value\n");
                return E_FAIL;
            }

            hres = RunSetupCommandW(ctx->hwnd, ctx->install_file, sect_name,
                    ctx->tmp_dir, NULL, NULL, RSC_FLAG_INF, NULL);
            if(FAILED(hres)) {
                WARN("RunSetupCommandW failed: %08x\n", hres);
                return hres;
            }
        }
    }

    if(default_install) {
        hres = RunSetupCommandW(ctx->hwnd, ctx->install_file, NULL, ctx->tmp_dir, NULL, NULL, RSC_FLAG_INF, NULL);
        if(FAILED(hres)) {
            WARN("RunSetupCommandW failed: %08x\n", hres);
            return hres;
        }
    }

    return S_OK;
}

static HRESULT install_cab_file(install_ctx_t *ctx)
{
    WCHAR tmp_path[MAX_PATH], tmp_dir[MAX_PATH];
    BOOL res = FALSE, leave_temp = FALSE;
    DWORD i;
    HRESULT hres;

    GetTempPathW(ARRAY_SIZE(tmp_path), tmp_path);

    for(i=0; !res && i < 100; i++) {
        GetTempFileNameW(tmp_path, NULL, GetTickCount() + i*17037, tmp_dir);
        res = CreateDirectoryW(tmp_dir, NULL);
    }
    if(!res)
        return E_FAIL;

    ctx->tmp_dir = tmp_dir;

    TRACE("Using temporary directory %s\n", debugstr_w(tmp_dir));

    hres = extract_cab_file(ctx);
    if(SUCCEEDED(hres)) {
        if(ctx->callback)
            IBindStatusCallback_OnProgress(ctx->callback, 0, 0, BINDSTATUS_INSTALLINGCOMPONENTS, ctx->install_file);

        switch(ctx->install_type) {
        case INSTALL_INF:
            hres = install_inf_file(ctx);
            break;
        case INSTALL_DLL:
            FIXME("Installing DLL, registering in temporary location\n");
            hres = setup_dll(ctx);
            if(SUCCEEDED(hres))
                leave_temp = TRUE;
            break;
        default:
            assert(0);
        }
    }

    if(!leave_temp)
        RemoveDirectoryW(ctx->tmp_dir);
    return hres;
}

static void update_counter(install_ctx_t *ctx, HWND hwnd)
{
    WCHAR text[100];

    if(--ctx->counter <= 0) {
        HWND button_hwnd;

        KillTimer(hwnd, ctx->timer);
        LoadStringW(urlmon_instance, IDS_AXINSTALL_INSTALL, text, ARRAY_SIZE(text));

        button_hwnd = GetDlgItem(hwnd, ID_AXINSTALL_INSTALL_BTN);
        EnableWindow(button_hwnd, TRUE);
    }else {
        WCHAR buf[100];
        LoadStringW(urlmon_instance, IDS_AXINSTALL_INSTALLN, buf, ARRAY_SIZE(buf));
        swprintf(text, buf, ctx->counter);
    }

    SetDlgItemTextW(hwnd, ID_AXINSTALL_INSTALL_BTN, text);
}

static BOOL init_warning_dialog(HWND hwnd, install_ctx_t *ctx)
{
    BSTR display_uri;
    HRESULT hres;

    if(!SetPropW(hwnd, ctxW, ctx))
        return FALSE;

    hres = IUri_GetDisplayUri(ctx->uri, &display_uri);
    if(FAILED(hres))
        return FALSE;

    SetDlgItemTextW(hwnd, ID_AXINSTALL_LOCATION, display_uri);
    SysFreeString(display_uri);

    SendDlgItemMessageW(hwnd, ID_AXINSTALL_ICON, STM_SETICON,
            (WPARAM)LoadIconW(0, (const WCHAR*)OIC_WARNING), 0);

    ctx->counter = 4;
    update_counter(ctx, hwnd);
    ctx->timer = SetTimer(hwnd, 1, 1000, NULL);
    return TRUE;
}

static INT_PTR WINAPI warning_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg) {
    case WM_INITDIALOG: {
        if(!init_warning_dialog(hwnd, (install_ctx_t*)lparam))
            EndDialog(hwnd, 0);
        return TRUE;
    }
    case WM_COMMAND:
        switch(wparam) {
        case ID_AXINSTALL_INSTALL_BTN: {
            install_ctx_t *ctx = GetPropW(hwnd, ctxW);
            if(ctx)
                ctx->cancel = FALSE;
            EndDialog(hwnd, 0);
            return FALSE;
        }
        case IDCANCEL:
            EndDialog(hwnd, 0);
            return FALSE;
        }
    case WM_TIMER:
        update_counter(GetPropW(hwnd, ctxW), hwnd);
        return TRUE;
    }

    return FALSE;
}

static BOOL install_warning(install_ctx_t *ctx)
{
    IWindowForBindingUI *window_iface;
    HWND parent_hwnd = NULL;
    HRESULT hres;

    if(!ctx->callback) {
        FIXME("no callback\n");
        return FALSE;
    }

    hres = IBindStatusCallback_QueryInterface(ctx->callback, &IID_IWindowForBindingUI, (void**)&window_iface);
    if(FAILED(hres))
        return FALSE;

    hres = IWindowForBindingUI_GetWindow(window_iface, &IID_ICodeInstall, &ctx->hwnd);
    IWindowForBindingUI_Release(window_iface);
    if(FAILED(hres))
        return FALSE;

    ctx->cancel = TRUE;
    DialogBoxParamW(urlmon_instance, MAKEINTRESOURCEW(ID_AXINSTALL_WARNING_DLG), parent_hwnd, warning_proc, (LPARAM)ctx);
    return !ctx->cancel;
}

static HRESULT install_file(install_ctx_t *ctx, const WCHAR *cache_file)
{
    BSTR path;
    HRESULT hres;

    TRACE("%s\n", debugstr_w(cache_file));

    ctx->cache_file = cache_file;

    if(!install_warning(ctx)) {
        TRACE("Installation cancelled\n");
        return S_OK;
    }

    hres = IUri_GetPath(ctx->uri, &path);
    if(SUCCEEDED(hres)) {
        const WCHAR *ptr, *ptr2, *ext;

        ptr = wcsrchr(path, '/');
        if(!ptr)
            ptr = path;
        else
            ptr++;

        ptr2 = wcsrchr(ptr, '\\');
        if(ptr2)
            ptr = ptr2+1;

        ctx->file_name = ptr;
        ext = wcsrchr(ptr, '.');
        if(!ext)
            ext = ptr;

        if(!wcsicmp(ext, cab_extW)) {
            hres = install_cab_file(ctx);
        }else {
            FIXME("Unsupported extension %s\n", debugstr_w(ext));
            hres = E_NOTIMPL;
        }
        SysFreeString(path);
    }

    return hres;
}

static void failure_msgbox(install_ctx_t *ctx, HRESULT hres)
{
    WCHAR buf[1024], fmt[1024];

    LoadStringW(urlmon_instance, IDS_AXINSTALL_FAILURE, fmt, ARRAY_SIZE(fmt));
    swprintf(buf, fmt, hres);
    MessageBoxW(ctx->hwnd, buf, NULL, MB_OK);
}

static HRESULT distunit_on_stop(void *ctx, const WCHAR *cache_file, HRESULT hresult, const WCHAR *error_str)
{
    install_ctx_t *install_ctx = ctx;

    TRACE("(%p %s %08x %s)\n", ctx, debugstr_w(cache_file), hresult, debugstr_w(error_str));

    if(hresult == S_OK) {
        hresult = install_file(install_ctx, cache_file);
        if(FAILED(hresult))
            failure_msgbox(ctx, hresult);
    }

    if(install_ctx->callback)
        IBindStatusCallback_OnStopBinding(install_ctx->callback, hresult, error_str);

    if(install_ctx->release_on_stop)
        release_install_ctx(install_ctx);
    return S_OK;
}

/***********************************************************************
 *           AsyncInstallDistributionUnit (URLMON.@)
 */
HRESULT WINAPI AsyncInstallDistributionUnit(const WCHAR *szDistUnit, const WCHAR *szTYPE, const WCHAR *szExt,
        DWORD dwFileVersionMS, DWORD dwFileVersionLS, const WCHAR *szURL, IBindCtx *pbc, void *pvReserved, DWORD flags)
{
    install_ctx_t *ctx;
    HRESULT hres;

    TRACE("(%s %s %s %x %x %s %p %p %x)\n", debugstr_w(szDistUnit), debugstr_w(szTYPE), debugstr_w(szExt),
          dwFileVersionMS, dwFileVersionLS, debugstr_w(szURL), pbc, pvReserved, flags);

    if(szDistUnit || szTYPE || szExt)
        FIXME("Unsupported arguments\n");

    ctx = heap_alloc_zero(sizeof(*ctx));
    if(!ctx)
        return E_OUTOFMEMORY;

    hres = CreateUri(szURL, 0, 0, &ctx->uri);
    if(FAILED(hres)) {
        heap_free(ctx);
        return E_OUTOFMEMORY;
    }

    ctx->callback = bsc_from_bctx(pbc);

    hres = download_to_cache(ctx->uri, distunit_on_stop, ctx, ctx->callback);
    if(hres == MK_S_ASYNCHRONOUS)
        ctx->release_on_stop = TRUE;
    else
        release_install_ctx(ctx);

    return hres;
}
