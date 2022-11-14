

const fs = require('fs');
const path = require('path');

const tags = [
  {
    id: 'personal',
    title: 'Personal 👨'
  }
]

const posts = [
  {
    url: "_posts/hello_world.html",
    title: "Hello, World!",
    tags: [ "personal" ]
  }
];

function createTagFile(tag) {
  const dir = path.resolve(path.join(__dirname, '..', 'posts'));

  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir);
  }


  const tagFileName = `tag_${tag.id}.html`
  createPostPage(path.join(__dirname, '..', 'posts', tagFileName), tag.id)
  return `/posts/${tagFileName}`;
}

function createTag(tag) {
  return `
    <a href="${createTagFile(tag)}"><button id="${tag.id}">${tag.title}</button></a>
  `;
}

function createPostServableFile(post) {
  const dir = path.resolve(path.join(__dirname, '..', 'posts'));

  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir);
  }

  const baseFilePath = path.join(__dirname, '..', post.url);
  const stats = fs.statSync(baseFilePath);
  const content = fs.readFileSync(baseFilePath);
  const fileName = post.url.substring(post.url.lastIndexOf('/'));
  const filePath = path.join(dir, fileName);
  fs.writeFileSync(filePath,`
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	  <meta charset="utf-8">
	  <link rel="stylesheet" href="/index.css">
    <link rel="stylesheet" href="/posts.css">
	  <title>Matthew Kosarek</title>
	  <link rel="shortcut icon" href="/favicon/favicon.ico" type="image/x-icon">
  </head>
  <body>
	  <header>
      <h1>Matthew Kosarek</h1>
      <nav>
        <ul>
          <li><a href='/'>&#127969; Home</a></li>
          <li><a href='/resume.html'>&#128216; CV</a></li>
          <li><a href="https://www.linkedin.com/in/matthew-kosarek/">🏢 LinkedIn</a></li>
          <li><a href='/posts.html'>&#128221; Posts</a></li>
        </ul>
      </nav>
	  </header>

    <article>
    <h2>${post.title}</h2>
    <h3>Created ${stats.birthtime.toLocaleString()}. Last updated: ${stats.mtime.toLocaleString()}</h3>
    ${content}
    </article>
  </body>
</html>
  `)
  return '/posts/' + fileName;
}

function createPostLink(post) {
  return `
    <li><a href="${createPostServableFile(post)}">${post.title}</a></li>
  `;
}

function createPostPage(outputFile, postFilter) {
  const output = `
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	  <meta charset="utf-8">
	  <link rel="stylesheet" href="/index.css">
    <link rel="stylesheet" href="/posts.css">
	  <title>Matthew Kosarek</title>
	  <link rel="shortcut icon" href="/favicon/favicon.ico" type="image/x-icon">
  </head>
  <body>
	  <header>
      <h1>Matthew Kosarek</h1>
      <nav>
        <ul>
          <li><a href='/'>&#127969; Home</a></li>
          <li><a href='/resume.html'>&#128216; CV</a></li>
          <li><a href="https://www.linkedin.com/in/matthew-kosarek/">🏢 LinkedIn</a></li>
          <li><a href='/posts.html'>&#128221; Posts</a></li>
        </ul>
      </nav>
	  </header>

    ${postFilter ? '' : `<section>
    <h2>Tags</h2>

    <div id='tag_list'>
        ${tags.map(createTag)}
    </div>
  </section`}

    <section>
    <h2>Posts</h2>
    <ul id='post_list'>
        ${posts.filter(post => !postFilter || post.tags.includes(postFilter)).map(createPostLink)}
    </ul>
    </section>
  </body>
</html>
`

  fs.writeFileSync(outputFile, output);
}

createPostPage(path.join(__dirname, '..', 'posts.html'));
