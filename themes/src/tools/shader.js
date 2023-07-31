/*
  Responsible for generating .h files from GLSL files.
 */


const path = require('path');
const fs = require('fs');
const directory = path.join(__dirname, "..", "_shaders");
const out_directory = path.join(__dirname, "..", "shaders");

const files = fs.readdirSync(directory);
files.forEach(file => {
  const filePath = path.join(directory, file);
  const text = String(fs.readFileSync(filePath));
  const splitText = text.split('\n');
  const splitName = file.split('.');
  const def = `SHADER_${splitName.join('_').toUpperCase()}`;
  let result = `#ifndef ${def} \n`;
  result += `#define ${def} \n`;
  result += `const char* ${def.toLowerCase()} = `
  splitText.forEach((line, index) => {
    result += "\"" + line + " \\n\"";
    if (index == splitText.length - 1)
      result += ";\n";
    else
      result += "\n";
  });
  result += '#endif\n';
  fs.writeFileSync(path.join(out_directory, splitName.join('_') + '.h'), result);
});
