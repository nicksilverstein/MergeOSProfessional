
include_directories(
    ${MERGEOS_SOURCE_DIR}
    ${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/arbiter
    ${MERGEOS_SOURCE_DIR}/sdk/lib/cmlib
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include/internal
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/drivers)

add_definitions(
    -D_NTOSKRNL_
    -D_NTSYSTEM_
    -DNTDDI_VERSION=0x05020400)

if(NOT DEFINED NEWCC)
    set(NEWCC FALSE)
endif()

if(NEWCC)
    add_definitions(-DNEWCC)
    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/cachesub.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/copysup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/fssup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/lazyrite.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/logsup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/mdlsup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/pinsup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/fault.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/swapout.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/data.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/reqtools.c)
else()
    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/cacheman.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/copy.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/fs.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/lazywrite.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/mdl.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/pin.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/cc/view.c)
endif()

list(APPEND SOURCE
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/io.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/cache/section/sptab.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmalloc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmapi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmboot.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmcheck.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmconfig.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmcontrl.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmdata.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmdelay.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmhook.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmhvlist.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cminit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmkcbncb.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmlazy.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmmapvw.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmnotify.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmparse.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmquota.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmse.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmsecach.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmsysini.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmvalche.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/cmwraprs.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/ntapi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/dbgk/dbgkobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/dbgk/dbgkutil.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/atom.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/callback.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/dbgctrl.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/efi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/event.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/evtpair.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/exintrin.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/fmutex.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/handle.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/harderr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/hdlsterm.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/init.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/interlocked.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/keyedevt.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/locale.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/lookas.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/mutant.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/profile.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/pushlock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/resource.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/rundown.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/sem.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/shutdown.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/sysinfo.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/time.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/timer.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/uuid.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/win32k.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/work.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/xipdisp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/zone.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/dbcsname.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/fastio.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/faulttol.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/filelock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/filter.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/filtrctx.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/fsfilter.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/fsrtlpc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/largemcb.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/mcb.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/name.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/notify.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/oplock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/pnp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/stackovf.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/tunnel.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fsrtl/unc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fstub/disksup.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fstub/fstubex.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fstub/halstub.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/fstub/translate.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/inbv/bootanim.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/inbv/inbv.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/inbv/inbvport.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/adapter.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/arcname.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/bootlog.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/controller.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/device.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/deviface.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/driver.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/error.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/file.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iocomp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/ioevent.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iofunc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iomdl.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iomgr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iorsrce.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iotimer.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/iowork.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/irp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/irq.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/ramdisk.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/rawfs.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/remlock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/symlink.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/util.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/iomgr/volume.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/arbiters.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/devaction.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/devnode.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/plugplay.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpdma.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpinit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpirp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpmgr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpnotify.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpreport.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnpres.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnproot.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr/pnputil.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/io/debug.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdapi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdbreak.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kddata.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdinit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdlock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdprint.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/kdtrap.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/apc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/balmgr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/bug.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/clock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/config.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/devqueue.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/dpc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/eventobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/except.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/freeze.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/gate.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/gmutex.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/ipi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/krnlinit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/mutex.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/procobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/profobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/queue.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/semphobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/spinlock.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/thrdobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/thrdschd.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/time.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/timerobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/wait.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/close.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/complete.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/connect.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/create.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/listen.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/port.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/reply.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/lpc/send.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/contmem.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/drvmgmt.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/dynamic.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/expool.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/hypermap.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/iosup.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/kdbg.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/largepag.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/mdlsup.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/mmdbg.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/mminit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/mmsup.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/ncache.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/pagfault.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/pfnlist.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/pool.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/procsup.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/section.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/session.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/special.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/sysldr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/syspte.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/vadnode.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/virtual.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/wslist.cpp
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/zeropage.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/balance.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/freelist.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/marea.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/mmfault.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/mminit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/pagefile.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/region.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/rmap.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/section.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/shutdown.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/devicemap.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obdir.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obhandle.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obinit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/oblife.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/oblink.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obname.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obref.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obsdcach.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obsecure.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ob/obwait.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/po/events.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/po/guid.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/po/poshtdwn.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/po/povolume.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/po/power.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/apphelp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/debug.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/job.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/kill.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/process.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/psmgr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/psnotify.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/query.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/quota.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/security.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/state.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/thread.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/win32.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/rtl/libsupp.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/rtl/misc.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/access.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/accesschk.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/acl.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/audit.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/client.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/objtype.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/priv.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/sd.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/semgr.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/sid.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/sqos.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/srm.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/subject.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/token.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/tokenadj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/tokencls.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/tokenlif.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/vf/driver.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/wmi/guidobj.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/wmi/smbios.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/wmi/wmi.c
    ${MERGEOS_SOURCE_DIR}/ntoskrnl/wmi/wmidrv.c)

if(DBG)
    list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/se/debug.c)
endif()

list(APPEND ASM_SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/zw.S)

if(ARCH STREQUAL "i386")
    list(APPEND ASM_SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/i386/fastinterlck_asm.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/i386/ioport.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/ctxswitch.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/trap.s
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/usercall_asm.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/zeropage.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/rtl/i386/stack.S)
    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/i386/cmhardwr.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/i386/kdx86.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/abios.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/cpu.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/context.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/exp.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/irqobj.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/kiinit.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/ldt.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/mtrr.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/patpge.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/thrdini.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/traphdlr.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/usercall.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/i386/v86vdm.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/i386/page.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/i386/procsup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/i386/init.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/i386/psctx.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/i386/psldt.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/vdm/vdmmain.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/vdm/vdmexec.c)
elseif(ARCH STREQUAL "amd64")
    list(APPEND ASM_SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/boot.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/ctxswitch.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/trap.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/usercall_asm.S
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/zeropage.S)
    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/i386/cmhardwr.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/i386/page.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/amd64/kdx64.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/context.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/cpu.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/except.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/interrupt.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/irql.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/kiinit.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/krnlinit.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/spinlock.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/thrdini.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/amd64/init.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/amd64/procsup.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/amd64/psctx.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/stubs.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/amd64/usercall.c)
elseif(ARCH STREQUAL "arm")
    list(APPEND ASM_SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ex/arm/ioport.s
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/boot.s
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/ctxswtch.s
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/stubs_asm.s
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/trap.s)
    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/config/arm/cmhardwr.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd64/arm/kdarm.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/cpu.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/exp.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/interrupt.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/kiinit.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/thrdini.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/trapc.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ke/arm/usercall.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/arm/page.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/mm/ARM3/arm/init.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/ps/arm/psctx.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/rtl/arm/rtlexcpt.c)
endif()

if(NOT _WINKD_)
    if(KDBG)
        add_definitions(-DKDBG)
    endif()

    if(ARCH STREQUAL "i386")
        list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/i386/kdserial.c)
        if(KDBG)
            list(APPEND ASM_SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/i386/kdb_help.S)
            list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/i386/i386-dis.c)
        endif()
    elseif(ARCH STREQUAL "amd64")
        list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/i386/kdserial.c)
        if(KDBG)
            list(APPEND ASM_SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/amd64/kdb_help.S)
            list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/i386/i386-dis.c)
        endif()
    elseif(ARCH STREQUAL "arm")
        list(APPEND SOURCE ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/arm/kdserial.c)
    endif()

    if(KDBG)
        list(APPEND SOURCE
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb.c
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb_cli.c
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb_cmdhist.c
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb_expr.c
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb_print.c
            ${MERGEOS_SOURCE_DIR}/ntoskrnl/kdbg/kdb_symbols.c)
    endif()

    list(APPEND SOURCE
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdio.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdmain.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdprompt.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdps2kbd.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdserial.c
        ${MERGEOS_SOURCE_DIR}/ntoskrnl/kd/kdterminal.c)

else()
    add_definitions(-D_WINKD_)
endif()
