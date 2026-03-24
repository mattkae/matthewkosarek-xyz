---
title: "Creating a WebAssembly Plugin System for Window Management (and beyond) in Miracle"
date: "2026-03-21"
tags: ["tech"]
---

## Plugins

Nothing quite makes you feel like you're using an exciting technology like a plugin system.
In fact, I would argue that this is one of the greatest draws of the web today, at least from
a business perspective.
Browsers provide a massive sandbox that runs some JavaScript, HTML, CSS, and more.
You get to throw arbitrary code files into this plugin stew to make something beautiful happen
(or at least make it so that your web page gets a 30 second load time).
Extensibility is a large part of what makes the modern web useful for business and tinkerers alike,
and it's also a large part of what makes software exciting in general.

But a sandbox as complex as a modern web browser isn't free. In fact - for better or for worse - it
is one of the most complex pieces of software that we have today. A few of these complications are:

- **Performance Overhead**: calling out to arbitrary code often involves a scripting language of some sort, which needs to be parsed, loaded, and executed by the host program. Native code has the luxury of being compiled for the target architecture, while plugin code *more likely than not* requires building at runtime.
- **
