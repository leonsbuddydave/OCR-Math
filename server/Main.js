var http = require('http');
var url = require('url');
var multipart = require('multipart');
var sys = require('sys');
var events = require('events');
var fs = require('fs');

http.createServer(function (Req, Res)
	{
		console.log('Starting OCRMath server...');

		switch( url.parse(Req.url).pathname )
		{
			case '/':
				DisplayForm(Req, Res);
				break;
			case '/upload':
				UploadFile(Req, Res);
				break;
			default:
				NotFound(Req, Res);
				break;
		}


		// parameter testing
		var Params = url.parse(Req.url, true);
		console.log(Params.query['name']);

	}).listen(13370, '192.168.1.6');

function DisplayForm(Req, Res)
{
	Res.setHeader(200, {'Content-Type' : 'text/html'});
	Res.sendBody(
		'<form action="/upload" method="post" enctype="multipart/form-data">'+
		'<input type="file" name="upload-file">'+
		'<input type="submit" value="Upload">'+
		'</form>'
	);
	Res.finish();
}

function WriteChunk(Reqest, FileDescriptor, Chunk, IsLast, ClosePromise)
{
	
	Request.pause();

	// Write chunk to file

	console.log('Writing chunk.');
	fs.write(FileDescriptor, Chunk).addCallback(function()
	{
		console.log('Wrote chunk.');
		
		request.resume();

		if (IsLast)
		{
			console.log('Closing file.');
			fs.close(FileDescriptor).addCallback(function()
			{
				console.log('Closed file.')

				ClosePromise.emitSuccess();
			});
		}
	});
}

function UploadFile(Req, Res)
{
	// Request body is binary
	Req.setBodyEncoding('binary');

	// Handle request as multipart
	var Stream = new multipart.Stream(Req);

	var ClosePromise = new events.Promise();

	Stream.addListener('part', function(Part)
	{
		console.log('Received part, name = ' + Part.name + ', filename = ' + Part.filename);

		var OpenPromise = null;

		// Add handler for a request part body chunk received
		Part.addListener('body', function(Chunk)
		{
			var Progress = (Stream.bytesReceived / Stream.bytesTotal * 100).toFixed(2);
			var MB = (Stream.bytesTotal / 1024 / 1024).toFixed(2);

			console.log('Uploading ' + MB + 'mb (' + Progress + '%)');

			if (OpenPromise == null)
			{
				console.log('Opening file...');
				OpenPromise = fs.open('./uploads/' + part.filename, process.O_CREAT | process.O_WRONLY, 0600);
			}

			// Add callback to execute after file is opened
			OpenPromise.addCallback(function(FileDescriptor)
			{
				WriteChunk(Req, FileDescriptor, Chunk, (Stream.bytesReceived == Stream.bytesTotal), ClosePromise);
			});
		});
	});

	Stream.addListener('complete', function()
	{
		console.log('Request complete.');

		// Wait until the file is closed
		ClosePromise.addCallback(function()
		{
			Res.sendHeader(200, { 'Content-Type' : 'text/plain' });
			Res.sendBody('Thanks for playing!');
			res.finish();

			console.log('Done.');
		});
	});
}

function NotFound(Req, Res)
{
	Res.sendHeader(404, { 'Content-Type' : 'text/plain' });
	Res.sendBody('Oops.');
	res.finish();
}
