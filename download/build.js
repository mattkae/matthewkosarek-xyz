

const fs = require('fs');
const childProcess = require('child_process');

const resumeHtml = fs.readFileSync('../resume.html').toString();
const resumeCss = fs.readFileSync('./resume_dl.css').toString();

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
      <script src="https://twemoji.maxcdn.com/2/twemoji.min.js?11.2"></script>
      <script>window.onload = function () { twemoji.parse(document.body);}</script>
<style>
${resumeCss}
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
