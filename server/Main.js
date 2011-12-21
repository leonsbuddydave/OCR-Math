var http = require('http');
var url = require('url');

http.createServer(function (req, res)
	{
		res.writeHead(200, { 'Content-Type' : 'text/plain' } );
		console.log(req.url);

		var params = url.parse(req.url, true);
		console.log(params.query['name']);

		res.end('Hello, world!\n');
	}).listen(13370, '192.168.1.6');
console.log('Starting OCRMath server...');

