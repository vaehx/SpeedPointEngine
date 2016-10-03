<map version="freeplane 1.2.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="SpeedPoint" LOCALIZED_STYLE_REF="AutomaticLayout.level.root" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1381328608965"><hook NAME="MapStyle" zoom="1.199">

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<hook NAME="AutomaticEdgeColor" COUNTER="9"/>
<node TEXT="Physics" POSITION="right" ID="ID_1890322144" CREATED="1381327666246" MODIFIED="1381328547158" HGAP="14" VSHIFT="-49">
<edge COLOR="#ffff00"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_995481125" MIDDLE_LABEL="Damage" STARTINCLINATION="-15;-69;" ENDINCLINATION="-22;-73;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_219159635" STARTINCLINATION="36;-3;" ENDINCLINATION="37;-2;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
<node TEXT="Sound" POSITION="right" ID="ID_219159635" CREATED="1381327663247" MODIFIED="1381329942701" HGAP="46" VSHIFT="-43">
<edge COLOR="#00ffff"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1684527796" STARTINCLINATION="241;0;" ENDINCLINATION="241;0;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
<node TEXT="Light" POSITION="right" ID="ID_920494298" CREATED="1381327646186" MODIFIED="1381329944367" HGAP="48" VSHIFT="-9">
<edge COLOR="#ff0000"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1684527796" STARTINCLINATION="157;0;" ENDINCLINATION="144;0;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_865587496" STARTINCLINATION="-41;-65;" ENDINCLINATION="-18;-39;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
<node TEXT="Geometry" POSITION="left" ID="ID_995481125" CREATED="1381327660416" MODIFIED="1381328463343" HGAP="114" VSHIFT="-39">
<edge COLOR="#ff00ff"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_865587496" STARTINCLINATION="6;12;" ENDINCLINATION="34;-14;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
<node TEXT="Animation" ID="ID_1856156111" CREATED="1381328451218" MODIFIED="1381328504903" HGAP="25" VSHIFT="19"/>
</node>
<node TEXT="Renderer" POSITION="left" ID="ID_865587496" CREATED="1381327650320" MODIFIED="1381328039087" HGAP="67" VSHIFT="-28">
<edge COLOR="#0000ff"/>
</node>
<node TEXT="Resources" POSITION="left" ID="ID_385335552" CREATED="1381327655584" MODIFIED="1381328049560" HGAP="48" VSHIFT="-14">
<edge COLOR="#00ff00"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_865587496" STARTINCLINATION="51;0;" ENDINCLINATION="51;0;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
<node TEXT="Artificial Intelligence" POSITION="right" ID="ID_292070784" CREATED="1381329920063" MODIFIED="1381329940888" HGAP="35" VSHIFT="20">
<edge COLOR="#007c00"/>
</node>
<node TEXT="Scripting" POSITION="right" ID="ID_1684527796" CREATED="1381327676845" MODIFIED="1381328485975" HGAP="34" VSHIFT="20">
<edge COLOR="#7c0000"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1856156111" STARTINCLINATION="0;70;" ENDINCLINATION="91;202;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
</node>
</map>
