---
title: "No More JetBrains Products for Me"
date: "2026-05-18"
tags: ["tech"]
---

After seeing that Zed [recently released](https://zed.dev/blog/zed-1-0) v1 of their editor, I figured
that now was a good of a time as any to give it a try. And while there are some quirks on Linux (specifically
a flickering bug on Wayland that was recently fixed), I feel comfortable running it everyday on my main
machine. The defaults are mostly sane, the editor is fast and responsive, and the compatibility
with much of Visual Studio Code's environment is great. Even the AI integration is tasteful and not
terribly in my way. Zed will be my primary editor going forward.

And this feels somewhat like a breakup. For a long time, I've been paying JetBrains ~$85 a year for their IDE.
I've been using CLion for all of my day-to-day work for years now. The unfortunate part is that
I want to love these products. I find the UI easy to understand. I find their defaults to be sane. The tooling allows
me to debug my problems with fantastic insight. All of the core ideas of JetBrains products are solid and
well executed. For this reason, I really want to love them. I want to use CLion every day.

But there is one teeny tiny problem: <u>**the tool is so fricken slow**</u>.

It is so remarkably slow, and I cannot begin to understand these people that are telling me that it runs fast.
Granted, I tend to run older hardware, but it seems that *most* other programs on my machine run
fast and happy, yet CLion finds a way to stutter and eat my CPU/RAM every step of the way while also making
simple editing tasks feel sluggish.

Here is a tiny list of some the frustrations that I've encountered:

- <u>I cannot instantly create a new file.</u> I have to go through some "Create File" popup that *literally* has
  a loading screen associated with it. Why do I have to **load** to create a new file? The slow down does not
  happen all of the time, but enough that I hesitate when wanting to create a new file.
- <u>The startup times are just abysmal.</u> It makes me not want to open my editor, which makes me hesitate to start doing
  work. If I have to sit their and stare at a splash screen at all, then I think something is wrong.
- <u>Switching projects has abysmal performance</u> as well.
- Remote Development has some weird quirks where - especially on slow machines - I will get disconnected abruptly.
- I cannot for the life of me understand why it keeps re-indexing my codebase in certain circumstances. Perhaps
  this is some on-again off-again bug, but it comes back to bite me constantly.
- <u>The install is gigantic on disk</u>, leading me to avoid it on my older machines where space is limited.

When all of these tiny issues come together, it makes me **NOT** want to program. I don't want to sit around and
wait for startup times to get my ideas onto the screen. I don't want to worry that my CPU or RAM is going to be exhaused
and I am going to have to restart my machine. I want to open my editor and immediately enter a flow state. I want the
tooling to assist me when useful, and stay out of my way when not.

**TLDR**: For this reason, my relationship with JetBrains is over, at least for now. Zed is the way forward for me, and I expect
to be a happier programmer because of it.
