# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```sh
npm install          # install dependencies (first time)
make build           # full build: WASM themes + Astro static site → dist/
make dev             # start Astro dev server on http://localhost:4321
make copy-themes     # build WASM themes and copy to public/themes/
make clean           # remove dist/ and public/themes/
./upload.sh          # deploy to production via rsync
```

WASM theme build requires Emscripten SDK and Meson. If you only need the Astro site, `npm run build` skips themes.

No linting, testing, or CI is configured.

## Architecture

Astro 5 static site for https://matthewkosarek.xyz. No JS framework — pure Astro components with inline `<script>` tags.

### Blog Posts

Posts use **Astro content collections** with Markdown files in `src/content/posts/`. Each post has frontmatter: `title`, `date`, `tags`. The collection schema is in `src/content/config.ts`.

To add a post: create `src/content/posts/<slug>.md` with the frontmatter fields above.

Post routes are generated via `src/pages/posts/[slug].astro` using `getStaticPaths()`. The listing page at `src/pages/posts/index.astro` supports tag filtering.

RSS feed is generated at `/posts/feed.xml` by `src/pages/posts/feed.xml.ts` using `@astrojs/rss` + `marked`.

### WASM Themes

C++17 compiled to WebAssembly via Emscripten + Meson (`themes/` subproject). Renders seasonal WebGL2 animations on the homepage canvas. GLSL shaders in `themes/src/_shaders/` are converted to C++ at build time. Output lands in `themes/dist/` and gets copied to `public/themes/dist/`.

### Dark/Light Theming

CSS custom properties defined in `src/styles/index.css`. Supports system preference via `prefers-color-scheme` and manual toggle via `data-theme` attribute on `<html>`. An inline script in `BaseLayout.astro` reads `localStorage('theme')` before paint to prevent FOUC.

### Layouts

- `BaseLayout.astro` — HTML shell, nav, theme toggle, font loading, global CSS
- `PostLayout.astro` — extends BaseLayout, adds post-specific CSS and article title wrapper

### Styles

CSS files live in `src/styles/` and are imported by their respective layouts/pages:
- `index.css` — global styles, nav, theme canvas, responsive breakpoints (1280px, 960px)
- `post.css` — code blocks, Shiki dual-theme support, heading anchors
- `sitemap.css` — post listing, tag filters
- `resume.css` — resume-specific layout

### Components

- `OpenMoji.astro` — renders OpenMoji SVG icons by emoji code from `node_modules/openmoji/`

### Markdown Config

Astro markdown uses rehype-slug + rehype-autolink-headings for heading anchors, and Shiki with dual themes (github-light / dracula) for code highlighting (`astro.config.mjs`).
