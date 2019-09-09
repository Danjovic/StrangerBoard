const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<HTML>
  <HEAD>
      <TITLE>Stranger Board</TITLE>
  </HEAD>
    <body style="color:red; background-color:#071f2d; text-align:center">
   

  <h1>Stranger Board</h1>
  
     
<p><br>
 <form style="color:red; text-align:center;" METHOD="get" action="/sendMessage"  >
  Enter Your Phrase...<br>
  <input type="text" name="myPhrase" value = ""><br>
  <br><br>
  <input type="submit" value="Make it Glow!">  
</form> 


</BODY>
</HTML>
)rawliteral";

