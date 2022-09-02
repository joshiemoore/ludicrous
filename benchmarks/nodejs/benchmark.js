const http = require('http');


http.createServer( (req, res) => {
  if (req.url == '/') {
    res.writeHead(200, {'Content-Type': 'application/json'});
    res.write(JSON.stringify({'hello': 'world'}));
    res.end();
  }
}).listen(8000);
