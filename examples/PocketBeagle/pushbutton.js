#!/usr/bin/env node
var b = require('bonescript');
var button = "P2_3";

console.log('Hit ^C to stop');
b.pinMode(button, b.INPUT, 7, 'pulldown', 'fast', doAttach);

function doAttach(err, x) {
    if(err) {
        console.log('pinMode err = ' + err);
        return;
    }
    b.attachInterrupt(button, true, b.CHANGE, printStatus);
}

function printStatus(err, x) {
    if(err) {
        console.log('attachInterrupt err = ' + err);
        return;
    }
    if(x.attached) {
        console.log("Interrupt handler attached");
        return;
    }
    process.stdout.write('value = ' + x.value + ', err   = ' + err + '          \r');
}
