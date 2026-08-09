import { defineCollection, z } from 'astro:content';
import { file } from 'astro/loaders';

const posts = defineCollection({
  type: 'content',
  schema: z.object({
    title: z.string(),
    date: z.string(),
    tags: z.array(z.string()),
  }),
});

const readingEntry = z.object({
  title: z.string(),
  author: z.string().optional(),
  type: z.enum(['book', 'article']).default('book'),
  status: z.enum(['done', 'reading']).default('done'),
  isbn: z.string().optional(),
  url: z.string().url().optional(),
  note: z.string().optional(),
});

const reading = defineCollection({
  loader: file('src/content/reading.yaml'),
  schema: z.object({
    year: z.number(),
    entries: z.array(readingEntry),
  }),
});

export const collections = { posts, reading };
