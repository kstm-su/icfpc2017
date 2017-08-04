'use strict';

var net = require('net');
var exec = require('child_process').exec;

var client = new net.Socket();

var child;

var port = process.argv[2];
var path = process.argv[3];

var buffer = '';
var remains = 0;

function sendInput(input) {
    console.log('Command: ' + input);
    child.stdin.write(input);
}

client.connect(port, 'punter.inf.ed.ac.uk', function () {
    console.log('Connected');

    child = exec(path);
    child.stdout.on('data', function (data) {
        console.log('Sending: ' + data);
        client.write(data.length + ':' + data);
    });

    child.stderr.on('data', function (data) {
        console.log('On stderr: ' + data);
    });
});

client.on('data', function (data) {
    buffer += data;
    while (true) {
        if (remains == 0 && buffer.indexOf(':') != -1) {
            remains = parseInt(buffer, 10);
            buffer = buffer.slice(buffer.indexOf(':') + 1);
        } else if (remains == 0) {
            break;
        }

        if (buffer.length >= remains) {
            const input = buffer.slice(0, remains);
            buffer = buffer.slice(remains);
            remains = 0;
            sendInput(input);
        } else {
            break;
        }
    }
 
    console.log('Received: ' + data.toString());
});

client.on('close', function () {
    console.log('Connection closed');
});
