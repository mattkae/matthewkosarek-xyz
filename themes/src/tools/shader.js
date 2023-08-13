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
  let header_result = `#ifndef ${def} \n`;
  const filename = splitName.join('_');
  const header_name = filename + ".h";
  const cpp_name = filename + ".cpp";
  header_result += `#define ${def} \n`;
  header_result += `extern const char* ${def.toLowerCase()};\n`;
  header_result += '#endif\n';
  fs.writeFileSync(path.join(out_directory, header_name), header_result);
  
  let cpp_result = `#include "${header_name}"\n\n`
  cpp_result += `const char* ${def.toLowerCase()} = `
  splitText.forEach((line, index) => {
    cpp_result += "\"" + line + " \\n\"";
    if (index == splitText.length - 1)
      cpp_result += ";\n";
    else
      cpp_result += "\n";
  });
  fs.writeFileSync(path.join(out_directory,  cpp_name), cpp_result);
});
