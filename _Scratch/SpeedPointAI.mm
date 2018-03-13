<map version="freeplane 1.2.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="Artificial Intelligence" LOCALIZED_STYLE_REF="AutomaticLayout.level.root" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1381393540503"><hook NAME="MapStyle">

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
<hook NAME="AutomaticEdgeColor" COUNTER="5"/>
<node TEXT="Bot Entity" LOCALIZED_STYLE_REF="AutomaticLayout.level,1" POSITION="right" ID="ID_372291888" CREATED="1381393542242" MODIFIED="1381394869440">
<edge COLOR="#ff0000"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      derives from Model and entity
    </p>
  </body>
</html>

</richcontent>
<node TEXT="Knowledge memory" ID="ID_593799313" CREATED="1381393640577" MODIFIED="1381394853450">
<icon BUILTIN="help"/>
<icon BUILTIN="xmag"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      ?? Structure of a single knowledge thing ??
    </p>
  </body>
</html>

</richcontent>
</node>
<node TEXT="Properties" ID="ID_1830779158" CREATED="1381393651349" MODIFIED="1381393654344">
<node TEXT="party" ID="ID_584345538" CREATED="1381393655366" MODIFIED="1381393657004"/>
<node TEXT="age" ID="ID_207168841" CREATED="1381393658197" MODIFIED="1381393664515"/>
<node TEXT="run speed" ID="ID_1916590788" CREATED="1381394084313" MODIFIED="1381394098592"/>
<node TEXT="walk speed" ID="ID_1466117165" CREATED="1381394099770" MODIFIED="1381394101096"/>
</node>
<node TEXT="current target entity" ID="ID_137068392" CREATED="1381394106095" MODIFIED="1381394115752"/>
<node TEXT="Call of script events" ID="ID_1041273429" CREATED="1381394119332" MODIFIED="1381394129488">
<node TEXT="Fixed Events" ID="ID_387290397" CREATED="1381394271228" MODIFIED="1381394282906">
<node TEXT="OnEnemyDetect()" ID="ID_131714306" CREATED="1381394130322" MODIFIED="1381394500334"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      by default this contains Entity.Defeat()
    </p>
  </body>
</html>

</richcontent>
</node>
<node TEXT="OnPlayerInteracts()" ID="ID_153459131" CREATED="1381394136274" MODIFIED="1381394163760"/>
<node TEXT="OnTargetReached()" ID="ID_1233575418" CREATED="1381394208563" MODIFIED="1381394237392"/>
<node TEXT="OnStartDefeat()" ID="ID_1474594959" CREATED="1381394433504" MODIFIED="1381394464298"/>
</node>
<node TEXT="... extendable by application dev" ID="ID_523621887" CREATED="1381394242657" MODIFIED="1381394259692"/>
</node>
</node>
<node TEXT="Waynet" POSITION="left" ID="ID_123775265" CREATED="1381393604159" MODIFIED="1381393605875">
<edge COLOR="#0000ff"/>
<node TEXT="Waynode" ID="ID_1890621715" CREATED="1381393606819" MODIFIED="1381393610999"/>
<node TEXT="Path finding algorithm" ID="ID_1796746689" CREATED="1381393613644" MODIFIED="1381393629142">
<node TEXT="Corner rounding" ID="ID_1685893277" CREATED="1381394344105" MODIFIED="1381394370511"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      use hermite interpolation
    </p>
  </body>
</html>

</richcontent>
</node>
<node TEXT="do not collide with other entities" ID="ID_764195863" CREATED="1381394371626" MODIFIED="1381394399862"/>
</node>
</node>
<node TEXT="Custom trigger areas" POSITION="right" ID="ID_503130402" CREATED="1381394658567" MODIFIED="1381394763797">
<edge COLOR="#00ffff"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="80" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1041273429" STARTINCLINATION="75;0;" ENDINCLINATION="-19;41;" STARTARROW="NONE" ENDARROW="DEFAULT"/>
</node>
</node>
</map>
