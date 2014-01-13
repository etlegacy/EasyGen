EasyGen
=======

A full readme to come

NOTE
======
This program was originally created by Francesco Bancalà and due to him releasing the source we can now continue his work.

Included are his last notes which came with the source.

======

Hello,
	
maybe it's late but since I was asked lately, I'm gladly releasing the
EasyGen 1.42 popular terrain editor source code. Yay!

Nevertheless, the source is awkard. I really hope you'll manage to find it
useful, despite the many "WTF is this? Why in the name of God this was made this
way rather than in the elegant flawless typical way everyone is doing it?"

The answer is that this was my only and last big project in VC++ and I
didn't have the time to study all the stuff, I only wanted to get the editor
done because I loved Quake and more than that level editing! Luckly I liked
also solo-programming[1] and so I combined all this stuff toghether.

[1] definition of solo-programming: you do program alone without confronting
with others, so you end up making terrible mistakes or wasting time to do a
thing that would have required less than half of the time if done using a 3rd
party library for example.

I'm sorry if you hoped to find real useful information about compiling and
the source itself in here, but the reality is I don't remember a thing, too many
years and videgames has passed since. Things I remember:

	- I think I used VC6.0
	- I used a library to load images, BTNexgenIPL32
	- math/algebra stuff comes from my previous programming exercises just 4 fun
	- ... anything else you want to know, try to ask and I'll gladly answer if I
	know/remember the answer.

	Also please scan files for viruses, just in case.

	About licensing. Just do what you want with this code except hurting people.

Have fun!

Francesco Bancalà
bancaf@gmail.com
10 gen 2014, 21:33
--------------------------------------------------------------------------------
if you, unlike me, are a bitcoin millionaire, I accept donations :D
BTC: 12coA8JciHJ9QMYEnrVZhWaotQz72ty5v3

========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : EasyGen
========================================================================

//
// Fixed
//

AppWizard has created this EasyGen application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your EasyGen application.

EasyGen.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

EasyGen.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CEasyGenApp application class.

EasyGen.cpp
    This is the main application source file that contains the application
    class CEasyGenApp.

EasyGen.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

EasyGen.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\EasyGen.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file EasyGen.rc.

res\EasyGen.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.

EasyGen.reg
    This is an example .REG file that shows you the kind of registration
    settings the framework will set for you.  You can use this as a .REG
    file to go along with your application or just delete it and rely
    on the default RegisterShellFileTypes registration.



/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in EasyGen.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

EasyGenDoc.h, EasyGenDoc.cpp - the document
    These files contain your CEasyGenDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CEasyGenDoc::Serialize).

EasyGenView.h, EasyGenView.cpp - the view of the document
    These files contain your CEasyGenView class.
    CEasyGenView objects are used to view CEasyGenDoc objects.



/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named EasyGen.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC42XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC42DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////
