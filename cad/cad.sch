<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.7.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="mm" unit="mm" style="lines" multiple="1" display="yes" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="100" name="Blau" color="31" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
</parts>
<sheets>
<sheet>
<plain>
<dimension x1="0" y1="1.5" x2="0" y2="11.5" x3="-9.5" y3="6.5" textsize="1.778" layer="91"/>
<dimension x1="0" y1="13" x2="0" y2="23" x3="-9.5" y3="18" textsize="1.778" layer="91"/>
<dimension x1="10.5" y1="5" x2="10.5" y2="18" x3="1" y3="11.5" textsize="1.778" layer="91"/>
<wire x1="5" y1="1.5" x2="5" y2="5" width="0.1524" layer="95"/>
<wire x1="5" y1="5" x2="6.5" y2="5" width="0.1524" layer="95"/>
<wire x1="6.5" y1="5" x2="10" y2="5" width="0.1524" layer="95"/>
<wire x1="10" y1="5" x2="11.5" y2="5" width="0.1524" layer="95"/>
<wire x1="11.5" y1="5" x2="11.5" y2="1.5" width="0.1524" layer="95"/>
<wire x1="6.5" y1="5" x2="6.5" y2="8.5" width="0.1524" layer="95"/>
<wire x1="6.5" y1="8.5" x2="10" y2="8.5" width="0.1524" layer="95"/>
<wire x1="10" y1="8.5" x2="10" y2="5" width="0.1524" layer="95"/>
<wire x1="19.5" y1="13" x2="19.5" y2="15" width="0.1524" layer="95"/>
<wire x1="19.5" y1="15" x2="-8" y2="15" width="0.1524" layer="95"/>
<wire x1="-8" y1="15" x2="-8" y2="13" width="0.1524" layer="95"/>
<wire x1="-11.5" y1="15" x2="25.5" y2="15" width="0.1524" layer="100"/>
<wire x1="25.5" y1="15" x2="25.5" y2="16" width="0.1524" layer="100"/>
<wire x1="25.5" y1="16" x2="-11.5" y2="16" width="0.1524" layer="100"/>
<wire x1="19.7" y1="11.3" x2="25.7" y2="11.3" width="0.1524" layer="95"/>
<wire x1="25.7" y1="11.3" x2="25.7" y2="13.2" width="0.1524" layer="95"/>
<wire x1="25.7" y1="13.2" x2="22.7" y2="13.2" width="0.1524" layer="95"/>
<wire x1="22.7" y1="13.2" x2="22.7" y2="14.8" width="0.1524" layer="95"/>
<wire x1="22.7" y1="14.8" x2="25.7" y2="14.8" width="0.1524" layer="95"/>
<wire x1="25.7" y1="14.8" x2="25.7" y2="16.2" width="0.1524" layer="95"/>
<wire x1="25.7" y1="16.2" x2="22.7" y2="16.2" width="0.1524" layer="95"/>
<wire x1="22.7" y1="16.2" x2="22.7" y2="17.2" width="0.1524" layer="95"/>
<wire x1="22.7" y1="17.2" x2="26.7" y2="17.2" width="0.1524" layer="95"/>
<wire x1="26.7" y1="17.2" x2="26.7" y2="-3.4" width="0.1524" layer="95"/>
<wire x1="19.7" y1="11.3" x2="19.7" y2="1.7" width="0.1524" layer="95"/>
<wire x1="19.7" y1="1.7" x2="25.7" y2="1.7" width="0.1524" layer="95"/>
<wire x1="25.7" y1="1.7" x2="25.7" y2="-0.2" width="0.1524" layer="95"/>
<wire x1="25.7" y1="-0.2" x2="22.7" y2="-0.2" width="0.1524" layer="95"/>
<wire x1="22.7" y1="-0.2" x2="22.7" y2="-3.2" width="0.1524" layer="95"/>
<circle x="23.2" y="7.4" radius="2" width="0.1524" layer="95"/>
<dimension x1="23.2" y1="11.1" x2="26.7" y2="4.3" x3="24.95" y3="6.5" textsize="1.778" layer="95" dtype="horizontal"/>
</plain>
<instances>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<wire x1="0" y1="0" x2="25.5" y2="0" width="0.1524" layer="91"/>
<wire x1="25.5" y1="0" x2="25.5" y2="1.5" width="0.1524" layer="91"/>
<wire x1="25.5" y1="1.5" x2="0" y2="1.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<wire x1="0" y1="11.5" x2="25.5" y2="11.5" width="0.1524" layer="91"/>
<wire x1="25.5" y1="11.5" x2="25.5" y2="13" width="0.1524" layer="91"/>
<wire x1="25.5" y1="13" x2="0" y2="13" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
