<map version="freeplane 1.2.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="Scripting" LOCALIZED_STYLE_REF="AutomaticLayout.level.root" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1381392815366"><hook NAME="MapStyle">

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
<hook NAME="AutomaticEdgeColor" COUNTER="4"/>
<node TEXT="Script instance" POSITION="right" ID="ID_603588916" CREATED="1381392835583" MODIFIED="1381392846706">
<edge COLOR="#ff0000"/>
<node TEXT="Variable" ID="ID_1680374001" CREATED="1381392849335" MODIFIED="1381392853360"/>
<node TEXT="Function" ID="ID_541879460" CREATED="1381392854709" MODIFIED="1381392856253">
<node TEXT="Custom function" ID="ID_544641750" CREATED="1381392901314" MODIFIED="1381392904840"/>
<node TEXT="Event handlers" ID="ID_1083804956" CREATED="1381392905971" MODIFIED="1381393955863">
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_676038420" STARTINCLINATION="107;29;" ENDINCLINATION="140;0;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
</node>
<node TEXT="Class" ID="ID_786123454" CREATED="1381392857228" MODIFIED="1381392859178"/>
<node TEXT="Scope" ID="ID_1904194904" CREATED="1381393863612" MODIFIED="1381393870367"/>
</node>
<node TEXT="Compiler" POSITION="left" ID="ID_181433385" CREATED="1381393390053" MODIFIED="1381393498663" HGAP="40" VSHIFT="-40">
<edge COLOR="#0000ff"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_603588916" STARTINCLINATION="-22;-42;" ENDINCLINATION="-55;-150;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
<node TEXT="Entity" LOCALIZED_STYLE_REF="AutomaticLayout.level,1" POSITION="right" ID="ID_993570497" CREATED="1381393715980" MODIFIED="1381393904952" VSHIFT="30">
<edge COLOR="#ff00ff"/>
<node TEXT="Script class" ID="ID_676038420" CREATED="1381393721565" MODIFIED="1381393804135"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      provides Events
    </p>
  </body>
</html>

</richcontent>
</node>
<node TEXT="Identification Name" ID="ID_715689360" CREATED="1381393806140" MODIFIED="1381393927666" VSHIFT="20"/>
<node TEXT="(Abstract) Properties" ID="ID_524359331" CREATED="1381393818082" MODIFIED="1381393925747" VSHIFT="30"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      accessable in scope of class
    </p>
  </body>
</html>

</richcontent>
</node>
</node>
<node TEXT="Call of events" POSITION="left" ID="ID_1297936009" CREATED="1381393398578" MODIFIED="1381393441012" HGAP="30" VSHIFT="20">
<edge COLOR="#00ff00"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_181433385" STARTINCLINATION="83;0;" ENDINCLINATION="83;0;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
</node>
</map>
