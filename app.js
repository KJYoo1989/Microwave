var io = require('socket.io'),
    express = require('express'),
    fs = require('fs');


var app = express.createServer()
  , io = io.listen(app);

app.use(express.bodyParser());

app.post('/', function(req, res) {
   var time = req.param('time', 0);
   var stop = req.param('stop', null);
   console.log('stop: '+stop);
   console.log('time: '+time);

   // Sends message to browser      
   if (stop) {
     io.sockets.emit('stop video', stop);
   } else {
     io.sockets.emit('play video', time);
   }

   // Send a message back to arduino so it doesn't hang.
   res.send('Posted successfully');
});

app.get('/', function(req, res) {
  console.log('bad');
  res.send('bad request');
})


io.sockets.on('connection', function (socket) {
    socket.on('my event', function (msg) {
        console.log("DATA!!!");
    });
});

;

app.listen(8000);

