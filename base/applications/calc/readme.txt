              * * * * R E A C T O S - C A L C * * * *

INTRODUCTION
=============
This is MergeOS Calc, a scientific calculator for Win32 and Win64 systems.
I decided to start this project because the winecalc wasn't very usable (it's just my personal opinion) and the calculator included into Win95 and Win98 has some big limitations, like the missing support for 64 bit integers.
The user interface of MergeOS Calc is really similar to Microsoft calculator, so you should be able to use this replacement very quickly.

COMPILATION
============
You need MinGW for compiling MergeOS Calc.
Just launch MAKEALL.BAT from the source directory.
This will generate all executables for various configurations and platforms.

COMPILING THE HELP FILE
========================
MergeOS Calc uses HTMLHELP for opening the help file and generating the popups.
The Microsoft HTMLHELP Workshop is freely downloadable from Microsoft's site at:

http://www.microsoft.com/downloads/details.aspx?familyid=00535334-c8a6-452f-9aa0-d597d16580cc&displaylang=en

INSTALLATION
=============
CALC.CHM must be copied under %systemroot%\Help directory.
For Microsoft Windows's users, it can be \Windows\Help or \WinNT\Help.
For MergeOS's users, it must be \MergeOS\Help (at the moment the Help directory doesn't exists, so it must be created manually).

NOTES TO MERGEOS'S USERS
=========================
At the time of the write of this text, the newest version of MergeOS is 0.3.4.
While MergeOS Calc works fine with Microsoft's operating systems, there are some known issues with MergeOS:
* The keyboard shortcuts work, but there are still some issues.
* At startup, the focus is visible on the Inv control.
* The selection on the various radio buttons isn't displayed correctly.
* Into the aboutbox, the color around the MergeOS's logo isn't applied.
* Into the aboutbox, the scroll bar used into the read-only edit control doesn't work (seen into the italian version).
* For some reasons, the [RET] button of the statistical box doesn't set the focus on the calculator.
* Help file and help popups don't work.
* The floating point support into the shared runtime library is still unimplemented (ieee version only).

CLOSING WORDS
==============
it's a very short readme file, I know.
Unfortunately, I'm not very good at writing user's manuals.
Although MergeOS Calc is pretty simple and I still think you should be able to understand the basic operations yourself, a good manual would be a nice presentation for new users.
Then, the next step is the translation into other languages.
If you would like to help in some way on these tasks, you are welcome.

-------------------------------------------------------------------
Carlo Bramini, 19-05-2008
