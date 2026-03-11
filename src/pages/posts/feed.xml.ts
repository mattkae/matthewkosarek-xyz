import rss from '@astrojs/rss';
import { getCollection } from 'astro:content';
import { marked } from 'marked';
import type { APIContext } from 'astro';

export async function GET(context: APIContext) {
  const posts = await getCollection('posts');
  posts.sort((a, b) => b.data.date.localeCompare(a.data.date));

  return rss({
    title: "Matthew Kosarek's Blog",
    description: 'Updates and thoughts from Matthew Kosarek',
    site: context.site ?? 'https://matthewkosarek.xyz',
    items: posts.map(post => ({
      title: post.data.title,
      pubDate: new Date(post.data.date),
      link: `/posts/${post.slug}/`,
      content: marked(post.body) as string,
    })),
  });
}
