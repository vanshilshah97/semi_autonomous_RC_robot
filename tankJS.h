/*
 * tank mode UI HTML and javascript header
 * uses keyboard input for javascript
 * 
 */
const char tankbody[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
<head> <title> TankMode </title>
    <meta name="viewport" content="user-scalable=no">
    <link rel="shortcut icon" type="image/png"
 href="https://icons.iconarchive.com/icons/glyphish/glyphish/32/136-tractor-icon.png">
</head>
<body style="text-align: center;" >
Tank mode: Use Keys [W] [S] and [A]  [D] for levers. <br>[Q] for left arm and [Z] for right arm <br>
  <canvas id="canvas"></canvas><br>
  
  <button type="button" onclick="armLeft()" id="leftarm"> [S] left button </button> &nbsp;
  <button type="button" onclick="armRight()" id="rightarm"> right button [K] </button> <br>
    Command Sent: <span id="levers"> </span> <br>
  ESP32 Received: <span id="acknowledge">  </span> <br>
  <button type="button" onclick="manualcontrolmode()"> &nbsp; Manual Control &nbsp;  </button> &nbsp;
  <button type="button" onclick="wallfollowingmode()"> &nbsp; Wall following &nbsp;  </button> &nbsp;
  <button type="button" onclick="beacontrackingmode()"> &nbsp; Beacon Tracking &nbsp;  </button>&nbsp; 
  <button type="button" onclick="UDPcanfollowmode()"> &nbsp; UDP Can Follow &nbsp;  </button> <br><br>
  <span id="debug"> </span> <br><br><br>


<script>
  var canvas, ctx, width, height;
  var leftarmstate=0, rightarmstate=0;
  var stop=0;
  var leftstate=0, rightstate=0;
  var leverstate = [];
  
  canvas = document.getElementById('canvas');
  ctx = canvas.getContext('2d');          
  width = canvas.width;
  height = canvas.height;

// double check for inconsistency twice a second
  setInterval(myTimer, 500); 
  function myTimer() {  
    if (leftstate != leverstate[2] || rightstate != leverstate[3]) {
      sendState();
      document.getElementById("debug").innerHTML =  "fixing";
    }
  }
  
// keyboard
  document.addEventListener('keydown', keyDownHandler, true);
  document.addEventListener('keyup', keyUpHandler, true);
  window.addEventListener('focus', focusChange);
  window.addEventListener('blur', focusChange);

function focusChange() {
  leftstate=0; rightstate=0;
    leftarmstate=0; rightarmstate=0;
    updateState();
}

var keyboardCode = function(event) {
  var code;
  //if (event.repeat) return 0; // ignore repeating if held down
  if (event.key !== undefined) { code = event.key; } 
  else if (event.keyCode !== undefined) {code = event.keyCode;}
  return code;
};

drawLevers();

function updateState(){
  drawLevers();
  sendState();
}

function keyDownHandler(event) {
  var code = keyboardCode(event);
    if (code == 0) return; // repeating
    document.getElementById("debug").innerHTML =  code;
      
    if(code == 87 || code == 'w') { // Q key
      leftstate = -4;
    }
    if(code == 83 || code == 's') { // A key
      leftstate = 4;
    }
    if(code == 65 || code == 'a') { // P key
      rightstate = -4;
    }
    if(code == 68 || code == 'd') { // L key 
      rightstate = 4;
    }
    if(code == 81 || code == 'q') { // S key 
      leftarmstate = 1;
      document.getElementById("leftarm").style = "background-color:lime";
    }
    if(code == 90 || code == 'z') { // K key 
      rightarmstate = 1;
      document.getElementById("rightarm").style = "background-color:lime";
    }
    
    updateState();
}

function keyUpHandler(event) {
  var code =keyboardCode(event);
    
    if(code == 87 || code == 'w' ) { // Q key
      leftstate = 0;
    }    
    if(code ==83 || code == 's' ) { // A key
      leftstate = 0;
    }
    if(code == 65 ||  code == 'a' ) { // P key
      rightstate = 0 ; 
    }
    if(code == 68 ||  code == 'd' ) { // P key
      rightstate =0  ; 
    }
    if(code == 81 || code == 'q') { // S key 
      leftarmstate = 0;
      document.getElementById("leftarm").style = "background-color:#F0F0F0";
    }
    if(code == 90 || code == 'z') { // K key  
      rightarmstate = 0;
      document.getElementById("rightarm").style = "background-color:#F0F0F0";
    }
    
    updateState();
}

// DRAWING functions
  function background() {
    ctx.beginPath();
    ctx.fillStyle = '#c5c5c5';
    ctx.rect(width*1/4-5, height/2-50, 10, 100);
    ctx.rect(width*3/4-5, height/2-50, 10, 100);
    ctx.fill();
    ctx.fillStyle = 'black';
    ctx.fillText("[ W ]",width*1/4-8, height/2-55);
    ctx.fillText("[ S ]",width*1/4-8, height/2+60);
    ctx.fillText("[ A ]",width*3/4-8, height/2-55);
    ctx.fillText("[ D ]",width*3/4-8, height/2+60);
  }
  
  function knob(x,y) {
    var grd = ctx.createRadialGradient(x+10,y-10,3,x+10,y-10,50);
    ctx.beginPath();
    grd.addColorStop(0,"white");
    grd.addColorStop(1,"red");
    ctx.fillStyle = grd;
    ctx.arc(x, y, 20, 0, Math.PI * 2, true);
    ctx.fillStyle = grd;
    ctx.fill();
  }
  
  function drawLevers() {
    ctx.clearRect(0, 0, width, height);
    background();  
    knob(width*1/4, height/2+leftstate*10); // left knob
    knob(width*3/4, height/2+rightstate*10); // right knob
  }
  
// UI functions via AJAX to esp32
  function sendState() {
    var xhttp = new XMLHttpRequest();
    var str = "lever?val=";
    var res = str.concat(leftarmstate,",",rightarmstate,",",leftstate,",",rightstate);
    if (leftstate != 0 || rightstate != 0) {
      stop = 0;
    }
    if (leftstate == 0 && rightstate == 0) {
      if (stop==1){
          return;
      } 
    }
    document.getElementById("levers").innerHTML = res; 
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4) {
        if (this.status == 200) {
          resendflag = 0;         
          leverstate = this.responseText.split(",");
          document.getElementById("acknowledge").innerHTML = this.responseText;
        }  else  resendflag = 1
      }
    };
    xhttp.open("GET", res, true);
    xhttp.send();
    if (leftstate == 0 && rightstate == 0) {
      stop = 1;
    }
  }

  function manualcontrolmode() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        location.reload();
      }
    };
    xhttp.open("GET", "manualcontrolmode", true);
    xhttp.send();
  }

    function wallfollowingmode() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        location.reload();
      }
    };
    xhttp.open("GET", "wallfollowingmode", true);
    xhttp.send();
  }

    function beacontrackingmode() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        location.reload();
      }
    };
    xhttp.open("GET", "beacontrackingmode", true);
    xhttp.send();
  }

    function UDPcanfollowmode() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        location.reload();
      }
    };
    xhttp.open("GET", "UDPcanfollowmode", true);
    xhttp.send();
  }
</script>
</body>
</html>
)===";
