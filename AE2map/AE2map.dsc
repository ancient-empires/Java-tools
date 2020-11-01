%%version = "NA"
%%title = "Ancient Empire II - Map Editor"

title %%title
option decimalsep,","
option fieldsep,"|"
option priority,idle
DIRECTORY change,@path(%0)

if %1
	if @not(@path(%1))
		%1 = @path(%0)@path(%1)%1
	end
	%t = %%title" ("%1")"
else
	%t = %%title
end
if @winexists(%t)
	window activate,%t
	stop
end

inifile open,@path(%0)@name(%0)".ini"
%%lastmap = @iniread(autosaved,lastmap,@path(%0)"*.*")
%%grid = @iniread(autosaved,grid,)
%%newmapsize = @iniread(autosaved,newmapsize,"15 x 15")
%%texsel = @iniread(autosaved,texsel,18)
%%unitcolorswitch = 0

%%runcmdpath = @iniread(autosaved,runcmdpath,)


%%asso = @regread(root,"AE2mapfile\shell\open\command")
if @not(@equal(%%asso,@chr(34)@path(%0)@name(%0)".exe"@chr(34)" "@chr(34)"%1"@chr(34)))
	%%asso =
end

rem buffer map
list create,1

rem buffer temporaire
list create,2

rem subbuffer temporaire
list create,3

rem world rotation
list create,4
gosub list4_init

rem table for water & road fix
list create,5
gosub list5_init

rem log
list create,7

rem map en memoire ds list 8
rem list create,8

rem attribut des tiles
list create,9
gosub list9_init

  DIALOG CREATE,%%title,-1,0,750,579
REM *** Modifié par Dialog Designer le 08/11/02 - 06:46 ***
  DIALOG ADD,SHAPE,SHAPE1,2,2,553,553,black,black,,RECTANGLE
  DIALOG ADD,SHAPE,SHAPE2,2,559,82,553,black,black,,RECTANGLE
  DIALOG ADD,SHAPE,SHAPE3,7,645,54,54,black,black,,RECTANGLE
	dialog hide,SHAPE1
	dialog hide,SHAPE2
	dialog hide,SHAPE3
  DIALOG ADD,BUTTON,LOADMAP,136,648,96,24,Load
  DIALOG ADD,BUTTON,SAVEMAP,166,648,96,24,Save
  DIALOG ADD,BUTTON,NEWMAP,196,648,96,24,New
  DIALOG ADD,BUTTON,FIXWATER,241,648,96,24,"Basic water draw"
  DIALOG ADD,BUTTON,FIXROAD,271,648,96,24,"Basic road draw"
  DIALOG ADD,BUTTON,CHECKMAP,316,648,96,24,Check
  DIALOG ADD,BUTTON,RUNMAP,346,648,96,24,Run
  DIALOG ADD,CHECK,GRID,388,648,96,18,Use grid (slower),%%grid,,CLICK
  DIALOG ADD,CHECK,ASSO,536,648,96,18,".ae2 associated",%%asso,,CLICK
  DIALOG ADD,TEXT,TEXT1,561,7,,,TEXT1
  DIALOG ADD,TEXT,TEXT2,410,648,,,TEXT2
  DIALOG ADD,TEXT,TEXT3,460,648,,,TEXT3
  DIALOG ADD,TEXT,TEXT4,64,648,,,TEXT4
  DIALOG ADD,TEXT,TEXT5,490,648,,,TEXT5
  DIALOG SHOW

%%capturable_cnt = 0
%%capturable_max = 30

gosub capturable_cnt

dialog disable,savemap
%%needsave =
dialog disable,checkmap
dialog disable,fixwater
dialog disable,fixroad
dialog disable,runmap

%%createblocs = 1
%%clearblocs = 1
%%mapXbloc = 0
%%mapYbloc = 0
gosub initscreen

%%actualmapfile = "No map in memory"
%%mapmode = 1
gosub s_text
%%OLDmapmode = %%mapmode

rem draw tileset
%t = 0
%u = 0
%v = 0
repeat
	%a = @sum(562,@prod(%t,26))
	%b = @sum(5,@prod(%u,26))
	if @greater(2,@len(%v))
		%v = "0"%v
	end
	if @not(@equal(%v,28))
		DIALOG ADD,BITMAP,"t_"%v,%b,%a,26,26,@path(%0)"AE2map.bin|"@prod(%v,406),,CLICK
	end
	%v = @succ(%v)
	if @greater(%t,1)
		%t = 0
		%u = @succ(%u)
	else
		%t = @succ(%t)
	end
until @greater(%v,46)

%%create_unitset = 1
gosub draw_unitset

rem selection
if %%mapmode
	DIALOG ADD,BITMAP,"s_1",10,648,48,48,@path(%0)"AE2map.bin|"@prod(%%texsel,406),,CLICK,STRETCH
else
	DIALOG ADD,BITMAP,"s_1",10,648,48,48,@path(%0)"AE2map.bin|"@prod(@sum(%%unitsel,47),406),,CLICK,STRETCH
end

dialog show,SHAPE1
dialog show,SHAPE2
dialog show,SHAPE3

if %1
	goto loadmap
end

:evloop
wait "0,1"
%e = @event()
if %e
	%a = @substr(%e,1,2)
	rem t=texture, b=bloc, u=unité, s=selectionné
	if @equal(%a,"t_")@equal(%a,"b_")@equal(%a,"u_")@equal(%a,"s_")
		goto BLOCEVENT
	end
	goto %e
end
goto evloop

:close
inifile write,autosaved,lastmap,%%lastmap
inifile write,autosaved,grid,%%grid
inifile write,autosaved,newmapsize,%%newmapsize
inifile write,autosaved,texsel,%%texsel

STOP

:NEWMAPBUTTON
%a = @input("Enter size in units: W x H. Max:22, min:10 (width only)",%%newmapsize,)
if @not(@ok())
	goto evloop
end
option fieldsep,"x"
parse "%b;%c",%a
if @not(%c)
	warn "You didn't entered correctly the sizes. New map creation cancelled."
	goto evloop
end
if @greater(10,%b)
	beep
	goto NEWMAPBUTTON
end
if @greater(%c,22)
	beep
	goto NEWMAPBUTTON
end
if @greater(%b,22)
	beep
	goto NEWMAPBUTTON
end
list clear,1

%%mapXbloc = %b
%%mapYbloc = %c

%%newmapsize = %%mapXbloc" x "%%mapYbloc

rem entete
list add,1,0
list add,1,0
list add,1,0
list add,1,%%mapXbloc
list add,1,0
list add,1,0
list add,1,0
list add,1,%%mapYbloc

rem corps
%c = 0
repeat
	%c = @succ(%c)
	list add,1,%%texsel
until @equal(%c,@prod(%%mapXbloc,%%mapYbloc))

rem dummy ? sais pas trop ce que c 'est..
%a = 48
list add,1,0
list add,1,0
list add,1,0
list add,1,%a
%c = 0
repeat
	list add,1,0
	list add,1,0
	list add,1,0
	list add,1,2
	%c = @succ(%c)
until @equal(%c,%a)

rem données unités, ici entete et 0 unité
list add,1,0
list add,1,0
list add,1,0
list add,1,0

%%actualmapfile = "New_Map.ae2"
dialog enable,savemap
%%needsave = 1
dialog enable,checkmap
dialog enable,fixwater
dialog enable,fixroad
dialog enable,runmap

%%unitcount = 0
gosub s_unitinfo

gosub drawmap
if @not(%%mapmode)
	gosub unitdraw
end

goto evloop


:SAVEMAPBUTTON
%%mapfile = @filedlg("All files (*.*)|*.*|AE2 Map file (*.ae2)|*.ae2","Specify filename to save AE2 Map",%%actualmapfile,SAVE)
if @not(%%mapfile)
	goto evloop
end

%%lastmap = %%mapfile

if @file(%%mapfile,ahrs)@file(%%mapfile)
	if @not(@equal(%%actualmapfile,%%mapfile))
		if @ask("A map file with the same name already exist:"@cr()%%mapfile@cr()"Do you want to replace it ?")
			rem nop
		else
			goto SAVEMAPBUTTON
		end	
	end
end

%%actualmapfile_bak = %%actualmapfile
%%actualmapfile = %%mapfile
gosub checkmap
%%actualmapfile = %%actualmapfile_bak
if @not(@zero(%%warning))
	list savefile,7,@path(%0)@name(%0)".log"
	shell open,@path(%0)@name(%0)".log"
	if @ask("Do you want to save anyway ?")
		rem nop
	else
		goto evloop
	end
end

binfile open,1,%%mapfile,CREATE
if @not(@ok())
	warn "Cannot save to : "%%mapfile", file locked ?"
	goto evloop
end
list seek,1,0
repeat
	%t = @next(1)
	if %t
		binfile write,1,binary,%t
	end
until @not(%t)
binfile close,1

%%actualmapfile = %%mapfile
dialog disable,savemap
%%needsave =

dialog set,text1,%%actualmapfile
dialog title,%%title" ("%%actualmapfile")"

goto evloop


:LOADMAPBUTTON
%%mapfile = @filedlg("All files (*.*)|*.*|AE2 Map file (*.ae2)|*.ae2","Select AE2 Map File to load",%%lastmap)
if @not(%%mapfile)
	goto evloop
end

:loadmap
if %1
	%%mapfile = %1
	%1 =
end

%%lastmap = %%mapfile

if @both(@not(@file(%%mapfile,ahrs)),if @not(@file(%%mapfile)))
	info "Specified map not found:"@cr()%%mapfile
	goto evloop
end

%%mapfilesize = @file(%%mapfile,z)

if @not(@greater(%%mapfilesize,0))
	warn "Error, maps didn't seem valid in size!"
	goto evloop
end

rem map sX_dev_extreme_test_ok (22x22, 484 units, 3112b)
if @greater(%%mapfilesize,3584)
	warn "Error, map size seem too big to be a valid file!"
	goto evloop
end

binfile open,1,%%mapfile,read
rem %%mm = @binfile(read,1,hex,%%mapfilesize)
list clear,2
repeat
	%c = @binfile(read,1,binary,1)
	%c = @substr(%c,1,@pred(@len(%c)))
	list add,2,%c
until @binfile(eof,1)
binfile close,1

if @not(@equal(@count(2),%%mapfilesize))
	warn "Internal Error ! map in memory isn't the same length than the file : "@count(2)"/"%%mapfilesize" !"@cr()"Please report to author!"
	list clear,2
	goto evloop
end

list seek,2,0
%%o1 = @next(2)
%%o2 = @next(2)
%%o3 = @next(2)
%%o4 = @next(2)
%i = 0
%i = @sum(@prod(%%o1,16777216),%i)
%i = @sum(@prod(%%o2,65536),%i)
%i = @sum(@prod(%%o3,256),%i)
%i = @sum(%%o4,%i)
%x = %i

list seek,2,4
%%o1 = @next(2)
%%o2 = @next(2)
%%o3 = @next(2)
%%o4 = @next(2)
%i = 0
%i = @sum(@prod(%%o1,16777216),%i)
%i = @sum(@prod(%%o2,65536),%i)
%i = @sum(@prod(%%o3,256),%i)
%i = @sum(%%o4,%i)
%y = %i

if @greater(%x,22)
	warn "Error, map horizontal size seem too larger! : "%x
	list clear,2
	goto evloop
end
if @greater(%y,22)
	warn "Error, map vertical size seem too larger! : "%y
	list clear,2
	goto evloop
end

rem dummy
list seek,2,@sum(8,@prod(%x,%y))
%%o1 = @next(2)
%%o2 = @next(2)
%%o3 = @next(2)
%%o4 = @next(2)
%i = 0
%i = @sum(@prod(%%o1,16777216),%i)
%i = @sum(@prod(%%o2,65536),%i)
%i = @sum(@prod(%%o3,256),%i)
%i = @sum(%%o4,%i)
%%dummy_value = %i

rem unitcount
list seek,2,@sum(@sum(12,@prod(%x,%y)),@prod(%%dummy_value,4))
%%o1 = @next(2)
%%o2 = @next(2)
%%o3 = @next(2)
%%o4 = @next(2)
%i = 0
%i = @sum(@prod(%%o1,16777216),%i)
%i = @sum(@prod(%%o2,65536),%i)
%i = @sum(@prod(%%o3,256),%i)
%i = @sum(%%o4,%i)
%%unitcount = %i
gosub s_unitinfo

%%mapXbloc = %x
%%mapYbloc = %y
list assign,1,2
%%actualmapfile = %%mapfile
dialog enable,savemap
%%needsave =
dialog enable,checkmap
dialog enable,fixwater
dialog enable,fixroad
dialog enable,runmap

gosub drawmap
if @not(%%mapmode)
	gosub unitdraw
end

gosub checkmap
if @not(@zero(%%warning))
	list savefile,7,@path(%0)@name(%0)".log"
	shell open,@path(%0)@name(%0)".log"
end
goto evloop


:initscreen
rem info %%createblocs"(create)  "%%clearblocs"(clear)  "%%setposblocks"(setpos)"

%t = 0
%u = 0
repeat
	if %%grid
		%a = @sum(4,@prod(%t,25))
		%b = @sum(4,@prod(%u,25))
	else
		%a = @sum(14,@prod(%t,24))
		%b = @sum(14,@prod(%u,24))
	end

	if %%createblocs
		DIALOG ADD,BITMAP,"b_"%t"x"%u,%b,%a,24,24,,,CLICK
	end
	if %%setposblocks
		dialog setpos,"b_"%t"x"%u,%b,%a,24,24
	end
	if %%clearblocs
		if @not(@greater(%%mapXbloc,%t))@not(@greater(%%mapYbloc,%u))
			dialog hide,"b_"%t"x"%u
		end
	end
	if @greater(%t,20)
		%t = 0
		%u = @succ(%u)
	else
		%t = @succ(%t)
	end
until @greater(%u,21)
rem info %%createblocs"(create)  "%%clearblocs"(clear)  "%%setposblocks"(setpos)"
%%createblocs =
%%clearblocs =
%%setposblocks =
exit
_


:BLOCEVENT
%a = @substr(%e,1)
if @equal(%a,"t")
	%%BLOCEVENT = @substr(%e,3,4)
	goto BLOCEVENT_T
end
if @equal(%a,"b")
	%%BLOCEVENT = @substr(%e,3,@diff(@len(%e),5))
	goto BLOCEVENT_B
end
if @equal(%a,"u")
	%%BLOCEVENT = @substr(%e,3,@diff(@len(%e),5))
	goto BLOCEVENT_U
end
if @equal(%a,"s")
	goto BLOCEVENT_S
end
warn "Missing code here ;P"
goto evloop

:BLOCEVENT_T
%%texsel = %%BLOCEVENT
dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(%%texsel,406)
%%mapmode = 1
gosub s_text
if @not(@equal(%%mapmode,%%OLDmapmode))
	%%unitUNdraw = 1
	gosub unitdraw
	%%unitUNdraw =
end
%%OLDmapmode = %%mapmode
goto evloop

:BLOCEVENT_B
if %%mapmode
	option fieldsep,"X"
	parse "%a;%b",%%BLOCEVENT
	if @not(@greater(%%mapXbloc,%a))@not(@greater(%%mapYbloc,%b))
		beep
		goto evloop
	end
	%c = @sum(@prod(%%mapYbloc,%a),%b)
	list seek,1,@sum(%c,8)
	%%oldtex = @item(1)
	if @equal(1,@len(%%oldtex))
		%%oldtex = "0"%%oldtex
	end
	if @equal("LEFT",@click(B))
		dialog enable,savemap
		%%needsave = 1
		if @equal(%%oldtex,27)@greater(%%oldtex,36)
			%%capturable_cnt = @pred(%%capturable_cnt)
		end
		if @equal(%%texsel,27)@greater(%%texsel,36)
			%%capturable_cnt = @succ(%%capturable_cnt)
		end
		gosub capturable_cnt
		list delete,1
		list insert,1,%%texsel
		rem  - debug only!
		rem gosub drawmap
		dialog set,"B_"%%BLOCEVENT,@path(%0)"AE2map.bin|"@prod(%%texsel,406)
	end
	if @equal("RIGHT",@click(B))
		%%texsel = %%oldtex
		dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(%%texsel,406)
		gosub s_text
	end
else
	if @equal("LEFT",@click(B))
		dialog enable,savemap
		%%needsave = 1
		gosub UNITADD
	end
	if @equal("RIGHT",@click(B))
		gosub UNITDEL
	end
	if @equal("CENTER",@click(B))
		info TODO UNIT CENTER
	end
	rem info gosub unitdraw
end
goto evloop

:BLOCEVENT_U
if @equal("RIGHT",@click(B))
	%%unitcolorswitch = @succ(%%unitcolorswitch)
	if @greater(%%unitcolorswitch,3)
		%%unitcolorswitch = 0
	end
	gosub draw_unitset
end
%%unitsel = %%BLOCEVENT
%v = @sum(@prod(12,%%unitcolorswitch),%%unitsel)
if @greater(2,@len(%v))
	rem %v = "0"%v
end
dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(@sum(%v,47),406)
rem info %%BLOCEVENT@cr()%%unitcolorswitch@cr()%%unitsel@cr()@path(%0)"textures\unit_icons_"%%unitsel".bmp"
%%mapmode =
gosub s_text
if @not(@equal(%%mapmode,%%OLDmapmode))
	gosub unitdraw
end
%%OLDmapmode = %%mapmode

goto evloop

:BLOCEVENT_S
if %%mapmode
	list seek,4,0
	option fieldsep,">"
	if @equal("RIGHT",@click(B))
		if @match(4,%%texsel">")
			parse "%a;%b",@item(4)
			%%texsel = %b
			dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(%%texsel,406)
			gosub s_text
		end
	else
		if @match(4,">"%%texsel)
			parse "%a;%b",@item(4)
			%%texsel = %a
			dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(%%texsel,406)
			gosub s_text
		end
	end
	option fieldsep,"|"
	rem beep
	rem info TODO for mapmode
	goto evloop
end

if @equal("RIGHT",@click(B))
	%%unitcolorswitch = @succ(%%unitcolorswitch)
else
	%%unitcolorswitch = @pred(%%unitcolorswitch)
end
if @greater(0,%%unitcolorswitch)
	%%unitcolorswitch = 3
end
if @greater(%%unitcolorswitch,3)
	%%unitcolorswitch = 0
end
%v = @sum(@prod(12,%%unitcolorswitch),%%unitsel)
if @greater(2,@len(%v))
	rem %v = "0"%v
end
dialog set,"s_1",@path(%0)"AE2map.bin|"@prod(@sum(%v,47),406)
gosub s_text
gosub draw_unitset
rem info %%BLOCEVENT@cr()%%unitcolorswitch@cr()%%unitsel@cr()@path(%0)"textures\unit_icons_"%%unitsel".bmp"
rem info %%BLOCEVENT
rem dialog set,"s_1",@path(%0)"textures\unit_icons_"%%texsel".bmp"

goto evloop

:drawmap
rem info drawmap
if @zero(@count(1))
	exit
end
if @equal(%%mapmode,%%OLDmapmode)
	%%clearblocs = 1
	gosub initscreen
end
list seek,1,8
%t = 0
%u = 0
%c = 0
%%capturable_cnt = 0
repeat
	%v = @next(1)
	if @equal(%v,27)@greater(%v,36)
		%%capturable_cnt = @succ(%%capturable_cnt)
	end

	%c = @succ(%c)
	dialog set,"b_"%t"x"%u,@path(%0)"AE2map.bin|"@prod(%v,406)
	dialog show,"b_"%t"x"%u
	if @greater(%u,@diff(%%mapYbloc,2))
		%u = 0
		%t = @succ(%t)
	else
		%u = @succ(%u)
	end
until @equal(%c,@prod(%%mapXbloc,%%mapYbloc))
dialog set,text1,%%actualmapfile
dialog title,%%title" ("%%actualmapfile")"
dialog set,text2,"Map size: "@cr()%%mapXbloc" x "%%mapYbloc" ("@prod(%%mapXbloc,%%mapYbloc)") units."@cr()@prod(%%mapXbloc,24)" x "@prod(%%mapYbloc,24)" pixels."
gosub capturable_cnt
exit
_


:unitdraw
%%unitcount = 0
if @zero(@count(1))
	rem warn cannot drawunit without a map in memory!
	gosub s_unitinfo
	exit
else
	list assign,2,1
end
%a = @sum(11,@prod(%%mapXbloc,%%mapYbloc))
list seek,1,%a
%a = @succ(%a)
%t = @next(1)
%b = @prod(4,%t)

%a = @succ(%a)
%a = @succ(%a)
list seek,1,@sum(%a,%b)
%t = @next(1)
%u = @next(1)
%t = @prod(%t,256)
%%unitcount = @sum(%t,%u)
if @zero(%%unitcount)
	gosub s_unitinfo
	exit
end

%c = 0
repeat
	%c = @succ(%c)
	%%unittype = @next(1)

	%t = @next(1)
	%u = @next(1)
	%t = @prod(%t,256)
	%t = @sum(%t,%u)
	%%unitX = @div(%t,24)

	%t = @next(1)
	%u = @next(1)
	%t = @prod(%t,256)
	%t = @sum(%t,%u)
	%%unitY = @div(%t,24)

	%%unitcolor = 0
	if @greater(%%unittype,11)
		%%unitcolor = 1
	end
	if @greater(%%unittype,23)
		%%unitcolor = 2
	end
	if @greater(%%unittype,35)
		%%unitcolor = 3
	end

	if @greater(2,@len(%%unittype))
		rem %%unittype = "0"%%unittype
	end
	if %%unitUNdraw
		list seek,2,@sum(8,@sum(%%unitY,@prod(%%unitX,%%mapYbloc)))
		dialog set,"b_"%%unitX"x"%%unitY,@path(%0)"AE2map.bin|"@prod(@item(2),406)
		rem info debug@cr()%%mapXbloc", "%%mapYbloc@cr()%%unitX", "%%unitY@cr()@sum(%%unitY,@prod(%%unitX,%%mapYbloc))@cr()@item(2)
	else
		dialog set,"b_"%%unitX"x"%%unitY,@path(%0)"AE2map.bin|"@prod(@sum(%%unittype,47),406)
	end

	rem is hidden unit ?
	if @not(@greater(%%mapXbloc,%%unitX))@not(@greater(%%mapYbloc,%%unitY))
		if %%unitUNdraw
			dialog hide,"b_"%%unitX"x"%%unitY
		else
			dialog show,"b_"%%unitX"x"%%unitY
		end
		rem info hidden found@cr()%%mapXbloc", "%%mapYbloc@cr()%%unitX", "%%unitY
	end

	rem info @path(%0)"textures\unit_icons_"%%unittype".bmp"
	rem info '%%unitcount' '%%unittype' '%%unitX' '%%unitY'
until @equal(%c,%%unitcount)
rem info drawunit todo '%%unitcount'
gosub s_unitinfo
exit
_


:GRIDCLICK
%%grid = @dlgtext(grid)
%%setposblocks = 1
gosub initscreen
goto evloop


:UNITADD
%%unitcount = 0
option fieldsep,"X"
parse "%%unitXs;%%unitYs",%%BLOCEVENT
rem %a = fin map world
%a = @sum(11,@prod(%%mapXbloc,%%mapYbloc))
list seek,1,%a

rem lecture du mysterieux dummy (?)
%a = @succ(%a)
%t = @next(1)
%b = @prod(4,%t)

%a = @succ(%a)
%a = @succ(%a)
list seek,1,@sum(%a,%b)
%t = @next(1)
%u = @next(1)
%t = @prod(%t,256)
%%unitcount = @sum(%t,%u)
%%unitstart = @index(1)
%%unitalloc =

if @greater(%%unitcount,0)
	rem search for replace
	%c = 0
	repeat
		%%unittype = @next(1)
	
		%t = @next(1)
		%u = @next(1)
		%t = @prod(%t,256)
		%t = @sum(%t,%u)
		%%unitX = @div(%t,24)
	
		%t = @next(1)
		%u = @next(1)
		%t = @prod(%t,256)
		%t = @sum(%t,%u)
		%%unitY = @div(%t,24)

		if @both(@equal(%%unitX,%%unitXs),@equal(%%unitY,%%unitYs))
			if %%unitalloc
				warn "found 2 unit in the same location ! (loc: "%%unitXs" x "%%unitYs")"@cr()"Try to keep only one!"
			else
				%%unitalloc = %c
			end
		end
		%c = @succ(%c)
	until @equal(%c,%%unitcount)
end

if @not(%%unitalloc)
	list add,1,255
	list add,1,255
	list add,1,255
	list add,1,255
	list add,1,255
	%%unitalloc = %%unitcount
	%%unitcount = @succ(%%unitcount)
end

%%unittype = @sum(@prod(12,%%unitcolorswitch),%%unitsel)
list seek,1,@sum(%%unitstart,@prod(%%unitalloc,5))
rem list insert,1,TAEE
list put,1,@sum(@prod(12,%%unitcolorswitch),%%unitsel)

%i = @sum(12,@prod(%%unitXs,24))
%t = @div(%i,256)
%j = @prod(%t,256)
%u = @diff(%i,%j)

%%VDSDEMERDE = @next(1)
list put,1,%t

%%VDSDEMERDE = @next(1)
list put,1,%u

%i = @sum(12,@prod(%%unitYs,24))
%t = @div(%i,256)
%j = @prod(%t,256)
%u = @diff(%i,%j)

%%VDSDEMERDE = @next(1)
list put,1,%t

%%VDSDEMERDE = @next(1)
list put,1,%u

%i = %%unitcount
%t = @div(%i,256)
%j = @prod(%t,256)
%u = @diff(%i,%j)
list seek,1,@diff(%%unitstart,2)
list put,1,%t

%%VDSDEMERDE = @next(1)
list put,1,%u

rem info %t %u"(put)"@cr()index @index(1)

dialog set,"b_"%%unitXs"x"%%unitYs,@path(%0)"AE2map.bin|"@prod(@sum(%%unittype,47),406)
gosub s_unitinfo
exit
_

:UNITDEL
%%unitcount = 0
option fieldsep,"X"
parse "%%unitXs;%%unitYs",%%BLOCEVENT
%a = @sum(11,@prod(%%mapXbloc,%%mapYbloc))
list seek,1,%a
%a = @succ(%a)
%t = @next(1)
%b = @prod(4,%t)

%a = @succ(%a)
%a = @succ(%a)
list seek,1,@sum(%a,%b)
%t = @next(1)
%u = @next(1)
%t = @prod(%t,256)
%%unitcount = @sum(%t,%u)
%%unitstart = @index(1)
%%unitalloc =

if @greater(%%unitcount,0)
	rem search for replace
	%c = 0
	repeat
		%%unittype = @next(1)
	
		%t = @next(1)
		%u = @next(1)
		%t = @prod(%t,256)
		%t = @sum(%t,%u)
		%%unitX = @div(%t,24)
	
		%t = @next(1)
		%u = @next(1)
		%t = @prod(%t,256)
		%t = @sum(%t,%u)
		%%unitY = @div(%t,24)

		if @both(@equal(%%unitX,%%unitXs),@equal(%%unitY,%%unitYs))
			if %%unitalloc
				warn "found 2 unit in the same location ! (loc: "%%unitXs" x "%%unitYs")"@cr()"Try to keep only one! Refresh manually !"
			else
				%%unitalloc = %c
			end
		end
		%c = @succ(%c)
	until @equal(%c,%%unitcount)
end

if @not(%%unitalloc)
	rem aucune unité trouvée à cet emplacement
	beep
	gosub s_unitinfo
	exit
end

rem unit2del found, so need to save
dialog enable,savemap
%%needsave = 1

list seek,1,@sum(%%unitstart,@prod(%%unitalloc,5))
list delete,1
list delete,1
list delete,1
list delete,1
list delete,1

%%unitcount = @pred(%%unitcount)

%i = %%unitcount
%t = @div(%i,256)
%j = @prod(%t,256)
%u = @diff(%i,%j)
list seek,1,@diff(%%unitstart,2)
list put,1,%t

%%VDSDEMERDE = @next(1)
list put,1,%u

list seek,1,@sum(@sum(8,%%unitYs),@prod(%%mapYbloc,%%unitXs))
%v = @next(1)

dialog set,"b_"%%unitXs"x"%%unitYs,@path(%0)"AE2map.bin|"@prod(%v,406)

rem is hidden unit ?
if @not(@greater(%%mapXbloc,%%unitXs))@not(@greater(%%mapYbloc,%%unitYs))
	rem dialog hide,"b_"%%unitXs"x"%%unitYs
	dialog set,"b_"%%unitXs"x"%%unitYs,@path(%0)"AE2map.bin|"@prod(95,406)
	warn "Warning:"@cr()"Removing a hidden unit position may crash the game"@cr()"if played in story mode ! (Same for visible scripted units)"@cr()@cr()"It is recommended to put a unit back before saving !"
end
gosub s_unitinfo
exit
_


:list9_init
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,10
list add,9,10
list add,9,15
list add,9,5
list add,9,10
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,0
list add,9,15
list add,9,-1
list add,9,5
list add,9,5
list add,9,15
list add,9,15
list add,9,10
list add,9,15
list add,9,10
list add,9,10
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
list add,9,15
exit
_

:s_text
if %%mapmode
	list seek,9,%%texsel
	if @equal(%%texsel,1)@equal(%%texsel,39)@equal(%%texsel,41)@equal(%%texsel,43)@equal(%%texsel,45)
		dialog set,text4,"WORLD (animated)"@cr()"#"%%texsel" - "@hex(%%texsel,2)"h"@cr()"Defense "@item(9)
	else
		dialog set,text4,"WORLD"@cr()"#"%%texsel" - "@hex(%%texsel,2)"h"@cr()"Defense "@item(9)
	end
else
	dialog set,text4,"UNIT"@cr()"#"@sum(@prod(12,%%unitcolorswitch),%%unitsel)" - "@hex(@sum(@prod(12,%%unitcolorswitch),%%unitsel),2)"h"
end
exit
_

:draw_unitset
rem draw unitset
%t = 0
%u = 0
%v = 0
repeat
	%a = @sum(562,@prod(%t,26))
	%b = @sum(450,@prod(%u,26))
	if @greater(2,@len(%v))
		%v = "0"%v
	end
	if @not(@equal(%v,28))
		if %%create_unitset
			DIALOG ADD,BITMAP,"u_"%v,%b,%a,26,26,@path(%0)"AE2map.bin|"@prod(@sum(@sum(%v,@prod(12,%%unitcolorswitch)),47),406),,CLICK
		else
			dialog set,"u_"%v,@path(%0)"AE2map.bin|"@prod(@sum(@sum(%v,@prod(12,%%unitcolorswitch)),47),406)
		end
	end
	%v = @succ(%v)
	if @greater(%t,1)
		%t = 0
		%u = @succ(%u)
	else
		%t = @succ(%t)
	end
until @equal(%v,12)
%%create_unitset =
exit
_

:capturable_cnt
if @greater(%%capturable_cnt,%%capturable_max)
	dialog set,TEXT3,"Capturables: "@cr()%%capturable_cnt" / "%%capturable_max" (warning)"
	beep
else
	dialog set,TEXT3,"Capturables: "@cr()%%capturable_cnt" / "%%capturable_max
end
exit
_

:s_unitinfo
dialog set,text5,"Unitcount:"@cr()%%unitcount
exit
_

:list4_init
list add,4,"00>01"
list add,4,"01>02"

rem -> (transition)
list add,4,"02>12"

list add,4,"03>06"
list add,4,"04>08"
list add,4,"05>07"
list add,4,"06>09"

rem ->
list add,4,"07>11"
rem ->
list add,4,"08>00"
rem ->
list add,4,"09>14"

list add,4,"10>05"
list add,4,"11>03"
list add,4,"12>10"
list add,4,"13>04"
list add,4,"14>13"

rem ->
list add,4,"15>18"

list add,4,"16>15"

rem ->
list add,4,"17>16"
list add,4,"18>19"
list add,4,"19>17"
list add,4,"20>21"
list add,4,"21>22"

rem ->
list add,4,"22>26"

list add,4,"23>24"
list add,4,"24>25"

rem ->
list add,4,"25>29"

list add,4,"26>23"
list add,4,"27>37"
rem list add,4,"28>NA"
list add,4,"29>30"

rem ->
list add,4,"30>20"

list add,4,"31>32"

rem ->
list add,4,"32>33"

list add,4,"33>34"
list add,4,"34>35"
list add,4,"35>36"

rem ->
list add,4,"36>27"

list add,4,"37>39"
list add,4,"38>40"
list add,4,"39>41"
list add,4,"40>42"
list add,4,"41>43"
list add,4,"42>44"
list add,4,"43>45"
list add,4,"44>46"

rem ->
list add,4,"45>38"
rem ->
list add,4,"46>31"


exit
_

:CHECKMAPBUTTON
gosub checkmap
list savefile,7,@path(%0)@name(%0)".log"
rem info "Log file saved at: "@cr()@path(%0)@name(%0)".log"
shell open,@path(%0)@name(%0)".log"
goto evloop


:checkmap
%%warning = 0
if @zero(@count(1))
	exit
end
list clear,7
list clear,2

list add,7,"Map name: "%%actualmapfile

%t = %%actualmapfile
rem check if official map name (mo, s0, s1,...)
if @equal("m",@substr(@name(%t),1))@equal("s",@substr(@name(%t),1))
	if @equal(2,@len(@name(%t)))@equal(3,@len(@name(%t)))
		if @numeric(@substr(@name(%t),2,@len(%t)))
			rem map official name found
			%t =
		end
	end
end

rem official not found?
if %t
	if @not(@ext(%%actualmapfile))
		%%warning = @succ(%%warning)
		list add,2," "
		list add,2,"* WARNING: map doesn't have any extension."
		list add,2,"           It is recommended to add the *.ae2 extension to make it friendly to this map editor."
	end
else
	if @ext(%%actualmapfile)
		%%warning = @succ(%%warning)
		list add,2," "
		list add,2,"* WARNING: map have the extension: "@ext(%%actualmapfile)" when it using the internal game's name: "@name(%%actualmapfile)
		list add,2,"           It is recommended to remove its extension ("@ext(%%actualmapfile)") to make it friendly to the internal game's functions."
	end
end

list seek,1,0

%%mapexpectedsize = 0

rem report and check width and height
%a = @next(1)
%b = @next(1)
%c = @next(1)
%d = @next(1)
%i = 0
%i = @sum(@prod(%a,16777216),%i)
%i = @sum(@prod(%b,65536),%i)
%i = @sum(@prod(%c,256),%i)
%i = @sum(%d,%i)
%t = %i

%a = @next(1)
%b = @next(1)
%c = @next(1)
%d = @next(1)
%i = 0
%i = @sum(@prod(%a,16777216),%i)
%i = @sum(@prod(%b,65536),%i)
%i = @sum(@prod(%c,256),%i)
%i = @sum(%d,%i)
%u = %i

%%mapexpectedsize = @sum(%%mapexpectedsize,8)
%%mapexpectedsize = @sum(%%mapexpectedsize,@prod(%t,%u))

list add,7,"Map size: "%t" x "%u" ("@prod(%t,%u)") units/bytes. "@prod(24,%t)" x "@prod(24,%u)" pixels. "@count(1)" bytes (total)"
if @greater(%t,22)
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: invalid width: "%t
end
if @greater(%u,22)
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: invalid height: "%t
end

if @greater(10,%t)
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Map's width is lower than 10: "%t
	list add,2,"           This may prevent the map to work correctly on some versions"
	list add,2,"           Please try to always use a minimal 10 bloc to ensure to comply until 240x320 versions"
end

rem report and check map world
%v = @prod(%t,%u)
%c = 0
%%bluehouse = 0
%%redhouse = 0
%%greenhouse = 0
%%blackhouse = 0
%%blueking = 0
%%redking = 0
%%greenking = 0
%%blackking = 0
%%nahouse = 0
%%bluecastle = 0
%%redcastle = 0
%%greencastle = 0
%%blackcastle = 0
%%nacastle = 0
%%blueunit = 0
%%redunit = 0
%%greenunit = 0
%%blackunit = 0
%%capturable_cnt = 0
%%water_animated_cnt = 0
repeat
	%c = @succ(%c)
	%t = @next(1)
	if @equal(%t,1)
		%%water_animated_cnt = @succ(%%water_animated_cnt)
	end
	if @equal(%t,27)@equal(%t,37)
		%%nahouse = @succ(%%nahouse)
	end
	if @equal(%t,39)
		%%bluehouse = @succ(%%bluehouse)
	end
	if @equal(%t,41)
		%%redhouse = @succ(%%redhouse)
	end
	if @equal(%t,43)
		%%greenhouse = @succ(%%greenhouse)
	end
	if @equal(%t,45)
		%%blackhouse = @succ(%%blackhouse)
	end
	if @equal(%t,38)
		%%nacastle = @succ(%%nacastle)
	end
	if @equal(%t,40)
		%%bluecastle = @succ(%%bluecastle)
	end
	if @equal(%t,42)
		%%redcastle = @succ(%%redcastle)
	end
	if @equal(%t,44)
		%%greencastle = @succ(%%greencastle)
	end
	if @equal(%t,46)
		%%blackcastle = @succ(%%blackcastle)
	end
	if @equal(%t,27)@greater(%t,36)
		%%capturable_cnt = @succ(%%capturable_cnt)
	end
until @equal(%c,%v)

rem dummy ? sais pas trop ce que c 'est..
%a = @next(1)
%a = @next(1)
%a = @next(1)
%%dummy_value = @next(1)
%%dummy_size = @prod(%%dummy_value,4)

%%mapexpectedsize = @sum(%%mapexpectedsize,4)
%%mapexpectedsize = @sum(%%mapexpectedsize,%%dummy_size)

%c = 0
repeat
	%a = @next(1)
	%b = @next(1)
	%d = @next(1)
	%f = @next(1)
	if @not(@equal(%a%b%d%f,"0002"))
		info Debug_msg421@cr()"please contact author and send to him this map for analyse."
	end
	%c = @succ(%c)
until @equal(%c,%%dummy_value)

list add,7,"Dummy(?) size: "%%dummy_value" units. "%%dummy_size" bytes"

rem check and report units
%%o1 = @next(1)
%%o2 = @next(1)
%%o3 = @next(1)
%%o4 = @next(1)
%i = 0
%i = @sum(@prod(%%o1,16777216),%i)
%i = @sum(@prod(%%o2,65536),%i)
%i = @sum(@prod(%%o3,256),%i)
%i = @sum(%%o4,%i)
%%unitcount = %i

%%mapexpectedsize = @sum(%%mapexpectedsize,4)
%%mapexpectedsize = @sum(%%mapexpectedsize,@prod(%%unitcount,5))

if @not(@zero(%%unitcount))
	%c = 0
	repeat
		%c = @succ(%c)
		rem unité
		%a = @next(1)
		if @greater(%a,35)
			%%blackunit = @succ(%%blackunit)
		else
			if @greater(%a,23)
				%%greenunit = @succ(%%greenunit)
			else
				if @greater(%a,11)
					%%redunit = @succ(%%redunit)
				else
					%%blueunit = @succ(%%blueunit)
				end
			end
		end
		if @equal(%a,9)
			%%blueking = @succ(%%blueking)
		end
		if @equal(%a,21)
			%%redking = @succ(%%redking)
		end
		if @equal(%a,33)
			%%greenking = @succ(%%greenking)
		end
		if @equal(%a,45)
			%%blackking = @succ(%%blackking)
		end
		rem positions X-Y: 00 00, 00 00
		%b = @next(1)
		%b = @next(1)
		%b = @next(1)
		%b = @next(1)
	until @equal(%c,%%unitcount)
end

%s = " "
list add,7," "
list add,7," "
list add,7,"Blu"%s"Red"%s"Grn"%s"Blk"%s"Not"%s"Tot"
list add,7,"---"%s"---"%s"---"%s"---"%s"---"%s"---"

%t = %%bluehouse"|"%%redhouse"|"%%greenhouse"|"%%blackhouse"|"%%nahouse"|"@sum(%%bluehouse,%%redhouse,%%greenhouse,%%blackhouse,%%nahouse)"|capturable house(s)"
gosub fm
list add,7,%t

%t = %%bluecastle"|"%%redcastle"|"%%greencastle"|"%%blackcastle"|"%%nacastle"|"@sum(%%bluecastle,%%redcastle,%%greencastle,%%blackcastle,%%nacastle)"|capturable castle(s)"
gosub fm
list add,7,%t

%t = %%blueunit"|"%%redunit"|"%%greenunit"|"%%blackunit"|NA|"@sum(%%blueunit,%%redunit,%%greenunit,%%blackunit)"|unit(s)"
gosub fm
list add,7,%t

list add,7," "
list add,7,"Capturables total: "%%capturable_cnt" / "%%capturable_max
list add,7," "

%t = @sum(%%blueunit,%%redunit,%%greenunit,%%blackunit)
if @not(@equal(%%unitcount,%t))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: map seems corrupted! Declared unit does not match the actual units count: "%t", "%%unitcount
end

if @both(@zero(%%blueking),@greater(%%bluecastle,0))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Blue castle found but no blue commander!"
	list add,2,"           At least one commander is needed to allow his regen in case of death (without commmander, can't win a map!)"
end

if @both(@zero(%%redking),@greater(%%redcastle,0))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Red castle found but no red commander!"
	list add,2,"           At least one commander is needed to allow his regen in case of death (without commmander, can't win a map!)"
end

if @both(@zero(%%greenking),@greater(%%greencastle,0))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Green castle found but no green commander!"
	list add,2,"           At least one commander is needed to allow his regen in case of death (without commmander, can't win a map!)"
end

if @both(@zero(%%blackking),@greater(%%blackcastle,0))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Black castle found but no black commander!"
	list add,2,"           At least one commander is needed to allow his regen in case of death (without commmander, can't win a map!)"
end

if @greater(%%capturable_cnt,%%capturable_max)
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Capturables units too high: "%%capturable_cnt" / "%%capturable_max" (maximum) Map may crash!"
end

rem is story map ? m0, m1, m2, ... then cancel skirmish warnings
%t = @name(%%actualmapfile)
if @equal(%t,m0)@equal(%t,m1)@equal(%t,m2)@equal(%t,m3)@equal(%t,m4)@equal(%t,m5)@equal(%t,m6)@equal(%t,m7)
	rem nop
else
	rem check if color units without a color castle
	if @not(@greater(%%bluecastle,0))
		if @greater(@sum(%%bluehouse,%%blueunit),0)
			%%warning = @succ(%%warning)
			list add,2," "
			list add,2,"* WARNING (only if skirmish map): blue house(s) or unit(s) found without any blue castle!"
		end
	end
	if @not(@greater(%%redcastle,0))
		if @greater(@sum(%%redhouse,%%redunit),0)
			%%warning = @succ(%%warning)
			list add,2," "
			list add,2,"* WARNING (only if skirmish map): red house(s) or unit(s) found without any red castle!"
		end
	end
	if @not(@greater(%%greencastle,0))
		if @greater(@sum(%%greenhouse,%%greenunit),0)
			%%warning = @succ(%%warning)
			list add,2," "
			list add,2,"* WARNING (only if skirmish map): green house(s) or unit(s) found without any green castle!"
		end
	end
	if @not(@greater(%%blackcastle,0))
		if @greater(@sum(%%blackhouse,%%blackunit),0)
			%%warning = @succ(%%warning)
			list add,2," "
			list add,2,"* WARNING (only if skirmish map): black house(s) or unit(s) found without any black castle!"
		end
	end
end

if @greater(%%water_animated_cnt,15)
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Too much animated water may gives problems: "%%water_animated_cnt
	list add,2,"           Please try to reduce their number under 16 (you can use the non-animated one: tile #02)"
end

if @not(@equal(%%mapexpectedsize,@count(1)))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Map seems corrupted! Expected total size do not match with actual size:" %%mapexpectedsize" / "@count(1)
end

%%warning_water = 0
gosub fix_water
if @not(@zero(%%warning_water))
	%%warning = @succ(%%warning)
	list add,2," "
	list add,2,"* WARNING: Found "%%warning_water" water tiles with impossible configuration (see the map screen)."
end
%%warning_water =

if @not(@zero(@count(2)))
	list add,7," "
	list append,7,2
end

if @not(@zero(%%warning))
	warn %%warning" warning(s) have been found for the map:"@cr()%%actualmapfile@cr()@cr()"Please read the log and try to fix them before sharing your map!"
end

exit
_


:fm
%s = " "
option fieldsep,"|"
parse "%a;%b;%c;%d;%e;%f;%h",%t
if @greater(2,@len(%a))
	%a = "  "%a
else
	if @greater(3,@len(%a))
		%a = " "%a
	end
end
if @greater(2,@len(%b))
	%b = "  "%b
else
	if @greater(3,@len(%b))
		%b = " "%b
	end
end
if @greater(2,@len(%c))
	%c = "  "%c
else
	if @greater(3,@len(%c))
		%c = " "%c
	end
end
if @greater(2,@len(%d))
	%d = "  "%d
else
	if @greater(3,@len(%d))
		%d = " "%d
	end
end
if @greater(2,@len(%e))
	%e = "  "%e
else
	if @greater(3,@len(%e))
		%e = " "%e
	end
end
if @greater(2,@len(%f))
	%f = "  "%f
else
	if @greater(3,@len(%f))
		%f = " "%f
	end
end
%t = %a%s%b%s%c%s%d%s%e%s%f%s%h
%e =
exit
_


:fixwaterBUTTON
if @zero(@prod(%%mapXbloc,%%mapYbloc))
	goto evloop
end

dialog enable,savemap
%%needsave = 1
gosub fix_water
rem gosub drawmap
if @not(%%mapmode)
	gosub unitdraw
end
goto evloop


:fixroadBUTTON
if @zero(@prod(%%mapXbloc,%%mapYbloc))
	goto evloop
end

dialog enable,savemap
%%needsave = 1
gosub fix_road
rem gosub drawmap
if @not(%%mapmode)
	gosub unitdraw
end
goto evloop


:fix_water
list assign,3,1
%c = 0
%x = 0
%y = 0

repeat
	%u = 0
	%v = 0
	%%chk_h =
	%%chk_b =
	%%chk_g =
	%%chk_d =
	%%chk_hg =
	%%chk_hd =
	%%chk_bg =
	%%chk_bd =
	%c = @succ(%c)

	if @equal(%x,%%mapXbloc)
		warn "Warning, internal error, please contact author about error 482"
	end

	rem check haut +1
	if @not(@zero(%y))
		list seek,3,@sum(7,@sum(%y,@prod(%x,%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%u = @sum(%u,1)
		else
			%%chk_h = 1
		end
	else
		rem void found
		%u = @sum(%u,1)
	end

	rem check bas +2
	if @greater(@pred(%%mapYbloc),%y)
		list seek,3,@sum(9,@sum(%y,@prod(%x,%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%u = @sum(%u,2)
		else
			%%chk_b = 1
		end
	else
		rem void found
		%u = @sum(%u,2)
	end

	rem check gauche +4
	if @not(@zero(%x))
		list seek,3,@sum(8,@sum(%y,@prod(@pred(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%u = @sum(%u,4)
		else
			%%chk_g = 1
		end
	else
		rem void found
		%u = @sum(%u,4)
	end

	rem check droite +8
	if @greater(@pred(%%mapXbloc),%x)
		list seek,3,@sum(8,@sum(%y,@prod(@succ(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%u = @sum(%u,8)
		else
			%%chk_d = 1
		end
	else
		rem void found
		%u = @sum(%u,8)
	end

	rem check haut-gauche
	if @both(@not(@zero(%y)),@not(@zero(%x)))
		list seek,3,@sum(7,@sum(%y,@prod(@pred(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%v = @sum(%v,16)
		else
			%%chk_hg = 1
		end
	else
		rem void found
		%v = @sum(%v,16)
	end		

	rem check haut-droite
	if @both(@not(@zero(%y)),@greater(@pred(%%mapXbloc),%x))
		list seek,3,@sum(7,@sum(%y,@prod(@succ(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%v = @sum(%v,32)
		else
			%%chk_hd = 1
		end
	else
		rem void found
		%v = @sum(%v,32)
	end

	rem check bas-gauche
	if @both(@greater(@pred(%%mapYbloc),%y),@not(@zero(%x)))
		list seek,3,@sum(9,@sum(%y,@prod(@pred(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%v = @sum(%v,64)
		else
			%%chk_bg = 1
		end
	else
		rem void found
		%v = @sum(%v,64)
	end

	rem check bas-droite
	if @both(@greater(@pred(%%mapYbloc),%y),@greater(@pred(%%mapXbloc),%x))
		list seek,3,@sum(9,@sum(%y,@prod(@succ(%x),%%mapYbloc)))
		%t = @item(3)
		if @greater(15,%t)@equal(%t,29)@equal(%t,30)
			rem water found
			%v = @sum(%v,128)
		else
			%%chk_bd = 1
		end
	else
		rem void found
		%v = @sum(%v,128)
	end


	rem water found on up,down, left and right found. need to add diagonales for table (lame shortcut to reduce the table's size...)
	if @equal(%u,15)
		%u = @sum(%u,%v)
	end

	rem modify list1
	list seek,1,@sum(8,@sum(%y,@prod(%x,%%mapYbloc)))
	rem is water?
	%t = @item(1)
	if @greater(15,%t)
		rem check against invalid water position (half method only)
		if @both(@both(%%chk_h,@not(%%chk_g)),%%chk_bg)
			%u = break
		end
		if @both(@both(%%chk_h,@not(%%chk_d)),%%chk_bd)
			%u = break
		end
		if @both(@both(%%chk_g,@not(%%chk_h)),%%chk_hd)
			%u = break
		end
		if @both(@both(%%chk_g,@not(%%chk_b)),%%chk_bd)
			%u = break
		end
		if @both(@both(%%chk_b,@not(%%chk_g)),%%chk_hg)
			%u = break
		end
		if @both(@both(%%chk_b,@not(%%chk_d)),%%chk_hd)
			%u = break
		end
		if @both(@both(%%chk_d,@not(%%chk_h)),%%chk_hg)
			%u = break
		end
		if @both(@both(%%chk_d,@not(%%chk_b)),%%chk_bg)
			%u = break
		end
		list seek,5,0
		if @match(5,">"%u">")
			option fieldsep,">"
			parse "%%p1;%%p2;%%p3",@item(5)
			option fieldsep,"|"
			if @both(@equal(%%p3,0),@equal(%t,1)@equal(%t,2))
				rem no change for water 01 02
			else
				if @not(%%warning_water)
					list put,1,%%p3
					rem if @not(@equal(%%p3,%t))
						dialog set,"b_"%x"x"%y,@path(%0)"AE2map.bin|"@prod(%%p3,406)
					rem end
				end
			end
		else
			rem invalid water found and specified
			dialog set,"b_"%x"x"%y,@path(%0)"AE2map.bin|"@prod(96,406)
			if %%warning_water
				%%warning_water = @succ(%%warning_water)
			end
			rem warn debug: cannot found table value for: u%u x%x y%y
		end
	end
rem info x%x y%y@cr()u%u

	%y = @succ(%y)
	if @equal(%y,%%mapYbloc)
		%y = 0
		%x = @succ(%x)
	end
until @equal(%c,@prod(%%mapXbloc,%%mapYbloc))
exit
_

:list5_init
rem sol haut
list add,5,">14>11"
rem sol bas
list add,5,">13>6"

rem sol gauche
list add,5,">11>9"
rem sol droite
list add,5,">7>3"
rem sol haut-gauche
list add,5,">10>14"
rem sol haut-droite
list add,5,">6>13"
rem sol bas-gauche
list add,5,">9>8"
rem sol bas-droite
list add,5,">5>4"

rem sol diagonale bas-droite
list add,5,">127>5"

rem sol diagonale haut-droite
list add,5,">223>10"


rem sol diagonale bas-gauche
list add,5,">191>7"

rem sol diagonale haut-gauche
list add,5,">239>12"

rem water everywhere
list add,5,">255>0"

rem route à droite
list add,5,"#8#20"

rem route à gauche,droite
list add,5,"#12#20"

rem route à gauche
list add,5,"#4#20"

rem route en bas
list add,5,"#2#21"

rem route en haut
list add,5,"#1#21"

rem route bas,droite
list add,5,"#10#26"

rem route haut,bas
list add,5,"#3#21"

rem route haut,droite
list add,5,"#9#25"

rem route haut,gauche
list add,5,"#5#24"

rem route bas,gauche
list add,5,"#6#23"

rem route haut,bas,gauche,droite
list add,5,"#15#22"


rem addition for road with 3 sides (to keep?)
list add,5,"#7#22"
list add,5,"#11#22"
list add,5,"#13#22"
list add,5,"#14#22"



exit
_

:ASSOCLICK
%%asso = @dlgtext(asso)
if %%asso
	registry write,root,".ae2",,AE2mapfile
	registry write,root,"AE2mapfile\DefaultIcon",,@chr(34)@path(%0)@name(%0)".exe"@chr(34)",0"
	registry write,root,"AE2mapfile\shell\open\command",,@chr(34)@path(%0)@name(%0)".exe"@chr(34)" "@chr(34)"%1"@chr(34)
else
	registry delete,root,".ae2",,
 	registry delete,root,"AE2mapfile\DefaultIcon",,
	registry delete,root,"AE2mapfile\shell\open\command",,
	registry delete,root,"AE2mapfile\shell\open",,
	registry delete,root,"AE2mapfile\shell",,
	registry delete,root,"AE2mapfile",,
end
goto evloop

:runmapBUTTON
if @not(%%runcmdpath)
	%t = @filedlg("Batch file (*.cmd)|*.cmd|All files (*.*)|*.*","Select batch file to run map")
	rem user cancelled
	if @not(%t)
		goto evloop
	end
	%%runcmdpath = %t
	inifile write,autosaved,runcmdpath,%%runcmdpath
end

if @not(@file(%%runcmdpath,ahrs))
	warn "Specified batch file (for run map function) not found:"@cr()%%runcmdpath
	%%runcmdpath =
	goto runmapBUTTON
end

DIRECTORY change,@path(%%runcmdpath)
if @not(@equal(@curdir()"\",@path(%%runcmdpath)))
	%%runcmdpath =
	goto runmapBUTTON
end
%t = @name(%%actualmapfile)
if @equal(%t,m0)@equal(%t,m1)@equal(%t,m2)@equal(%t,m3)@equal(%t,m4)@equal(%t,m5)@equal(%t,m6)@equal(%t,m7)
	%%maptype = %t
else
	%%maptype = s0
end
binfile open,1,@path(%%runcmdpath)%%maptype,CREATE
DIRECTORY change,@path(%0)
if @not(@ok())
	warn "Cannot save to : "@path(%%runcmdpath)%%maptype", file locked ?"
	goto evloop
end
list seek,1,0
repeat
	%t = @next(1)
	if %t
		binfile write,1,binary,%t
	end
until @not(%t)
binfile close,1

if @not(@file(@path(%%runcmdpath)%%maptype,ahrs))
	warn Could not save the file at the specified location:@cr()@path(%%runcmdpath)%%maptype
	goto evloop
end

shell open,@chr(34)%%runcmdpath@chr(34),%%maptype
goto evloop


:fix_road
list assign,2,1
list seek,1,8
%c = 0
%x = 0
%y = 0
repeat
	%u = 0
	%v = 0
	%c = @succ(%c)

	if @equal(%x,%%mapXbloc)
		warn "Warning, internal error, please contact author about error 481"
	end

	rem check haut +1
	if @not(@zero(%y))
		list seek,2,@sum(7,@sum(%y,@prod(%x,%%mapYbloc)))
		%t = @item(2)
		if @both(@greater(%t,19),@greater(27,%t))@equal(%t,29)@equal(%t,30)
			rem road found
			%u = @sum(%u,1)
		end
	else
		rem void found
		%v = @sum(%v,1)
	end

	rem check bas +2
	if @greater(@pred(%%mapYbloc),%y)
		list seek,2,@sum(9,@sum(%y,@prod(%x,%%mapYbloc)))
		%t = @item(2)
		if @both(@greater(%t,19),@greater(27,%t))@equal(%t,29)@equal(%t,30)
			rem road found
			%u = @sum(%u,2)
		end
	else
		rem void found zero = need only once
		if @zero(%v)
			%v = @sum(%v,2)
		end
	end

	rem check gauche +4
	if @not(@zero(%x))
		list seek,2,@sum(8,@sum(%y,@prod(@pred(%x),%%mapYbloc)))
		%t = @item(2)
		if @both(@greater(%t,19),@greater(27,%t))@equal(%t,29)@equal(%t,30)
			rem road found
			%u = @sum(%u,4)
		end
	else
		rem void found
		if @zero(%v)
			%v = @sum(%v,4)
		end
	end

	rem check droite +8
	if @greater(@pred(%%mapXbloc),%x)
		list seek,2,@sum(8,@sum(%y,@prod(@succ(%x),%%mapYbloc)))
		%t = @item(2)
		if @both(@greater(%t,19),@greater(27,%t))@equal(%t,29)@equal(%t,30)
			rem road found
			%u = @sum(%u,8)
		end
	else
		rem void found
		if @zero(%v)
			%v = @sum(%v,8)
		end
	end

	rem if isolated, take void in count then
	if @equal(%u,1)@equal(%u,2)@equal(%u,4)@equal(%u,8)
		%u = @sum(%u,%v)
	end

	rem modify list1
	list seek,1,@sum(8,@sum(%y,@prod(%x,%%mapYbloc)))
	rem is road?
	%t = @item(1)
	if @both(@greater(%t,19),@greater(27,%t))@equal(%t,29)@equal(%t,30)
		list seek,5,0
		if @match(5,"#"%u"#")
			option fieldsep,"#"
			parse "%%p1;%%p2;%%p3",@item(5)
			option fieldsep,"|"
			if @equal(%t,29)@equal(%t,30)
				rem bridge rotation
				if @equal(%%p3,20)
					%%p3 = 29
				end
				rem bridge rotation
				if @equal(%%p3,21)
					%%p3 = 30
				end
				if @equal(%%p3,29)@equal(%%p3,30)
					rem nop
				else
					rem if bridge not H or V, don't convert it to road anyway and keep it untouched
					%%p3 = %t
				end
			end
			list put,1,%%p3
			if @not(@equal(%%p3,%t))
				dialog set,"b_"%x"x"%y,@path(%0)"AE2map.bin|"@prod(%%p3,406)
			end
		else
			rem warn debug: cannot found table value for: u%u x%x y%y
		end
	end
rem info x%x y%y@cr()u%u

	%y = @succ(%y)
	if @equal(%y,%%mapYbloc)
		%y = 0
		%x = @succ(%x)
	end
until @equal(%c,@prod(%%mapXbloc,%%mapYbloc))
exit
_
