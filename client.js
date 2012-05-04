var player = null;
// We use videos that are 3 seconds shorter then the initial input on microwave
// 11 seconds on microwave will grab 8 second video for perfect syncing
var videos = {
  8: 'LCUZ02JWaek',
 77: 'kfVsfOSbJY0',
 57: 'QH2-TGUlwu4',
 192: 'Lj05ZJLZvmA',
  27: 'NyRl_rgcMbU',
  64: 'SMltA39QgNY',
  75: 'USXJrvIugEo',
  232: 'SAjiiEOzOV8',
  156: 'iKqeDsJFo_k',
  377: 'Gt38D8_aoVI',
  'default': 'oHg5SJYRHA0'
}

function loadPlayer() {
  var params = { allowScriptAccess: "always", wmode: "transparent" };
  var atts = { id: "ytPlayer" };
  swfobject.embedSWF("http://www.youtube.com/apiplayer?version=3&enablejsapi=1&playerapiid=player1","video", "100%", "100%", "9", null, null, params, atts);
}

function loadVideo(video_id) {
  player = document.getElementById("ytPlayer");
  player.cueVideoById(video_id);
}





function pauseVideo() {
  if(player) {
    player.pauseVideo();
  }
}

function playVideo() {
  if(player) {
    player.playVideo();
  }
}

// Load the player when page loads!
loadPlayer();

// Google Youtube API calls this when Flash is ready
function onYouTubePlayerReady(p) {
  // Player is loaded so tell Node we are ready
  var socket = io.connect('http://kjyoo.co:8000');
  socket.on('play video', function(time) {
    var id = videos['default'];    
    time -= 2;
    if(videos[time]) {
      id = videos[time];
    }
    loadVideo(id);
    playVideo();
    console.log(time);
  });
  socket.on('stop video', function(data) {
    console.log(data);
    pauseVideo();
  });
}

