

const fs = require('fs');
const childProcess = require('child_process');

const resumeHtml = fs.readFileSync('../resume.html').toString();
const resumeCss = fs.readFileSync('../resume.css').toString();
const resumeDlCss = fs.readFileSync('./resume_dl.css').toString();

function getRidofTagsBetween(start, end, html) {
  let si = newHtml.indexOf(start);
  let ei = newHtml.indexOf(end, si) + end.length;
  return html.substring(0, si) + newHtml.substring(ei);
}

const newHeader = `
 <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	  <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta charset="UTF-8" />
      <style>
img.emoji {
   height: 1em;
   width: 1em;
   margin: 0 .05em 0 .1em;
   vertical-align: -0.1em;
}
</style>
<script src="https://unpkg.com/twemoji@latest/dist/twemoji.min.js" crossorigin="anonymous"></script>

<script>window.onload = function () { twemoji.parse(document.body);}</script>
      <link href="https://fonts.googleapis.com/css?family=Open+Sans:400,600,300" rel="stylesheet" type="text/css">
<style>
${resumeCss}
${resumeDlCss}
</style>
</head>
`

const startHead = resumeHtml.indexOf('<head>');
const endHead = resumeHtml.indexOf('</head>') + '</head>'.length;

let newHtml = resumeHtml.substring(0, startHead) + newHeader + resumeHtml.substring(endHead);

newHtml = getRidofTagsBetween('<header>', '</header>', newHtml);
newHtml = getRidofTagsBetween(`<div id='resume_button_container'>`, '</div>', newHtml);

fs.writeFileSync('./resume_dl.html', newHtml);

childProcess.exec('wkhtmltopdf --encoding \'UTF-8\' ./resume_dl.html cv.pdf ')
