import { defineConfig } from 'astro/config';

import sitemap from '@astrojs/sitemap';
import rehypeSlug from 'rehype-slug';
import rehypeAutolinkHeadings from 'rehype-autolink-headings';

export default defineConfig({
  output: 'static',
  site: 'https://matthewkosarek.xyz',
  integrations: [sitemap()],
  markdown: {
    rehypePlugins: [
      rehypeSlug,
      [rehypeAutolinkHeadings, {
        behavior: 'prepend',
        properties: { class: 'heading-anchor', ariaHidden: 'true', tabIndex: -1 },
        content: { type: 'text', value: '#' },
      }],
    ],
    shikiConfig: {
      themes: {
        light: 'github-light',
        dark: 'dracula',
      },
      defaultColor: false,
    },
  },
});
