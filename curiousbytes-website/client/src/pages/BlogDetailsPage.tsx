import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom";
import { semanticColors } from "../constants/theme";

interface Blog {
  id: string;
  title: string;
  slug?: string;
  author_name?: string;
  cover_image?: string;
  content?: string;
  published_at?: string;
}

const BlogDetailsPage: React.FC = () => {
  const { slug } = useParams();
  const [blog, setBlog] = useState<Blog | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const fetchBlog = async () => {
      setLoading(true);
      try {
        const res = await fetch(`/api/blogs/${slug}`);
        if (!res.ok) throw new Error("Failed to fetch blog");
        const data = await res.json();
        setBlog(data);
      } catch (err: any) {
        setError(err.message || "Failed to load blog");
      } finally {
        setLoading(false);
      }
    };

    fetchBlog();
  }, [slug]);

  if (loading) {
    return <p className="text-center text-gray-500 mt-10">Loading blog...</p>;
  }

  if (error) {
    return <p className="text-center text-red-500 mt-10">{error}</p>;
  }

  if (!blog) {
    return <p className="text-center text-gray-500 mt-10">Blog not found.</p>;
  }

  return (
    <div className="px-4 py-10 max-w-3xl mx-auto">
      {blog.cover_image && (
        <img
          src={blog.cover_image}
          alt={blog.title}
          className="w-full h-64 object-cover rounded-lg mb-6"
        />
      )}

      <h1 className="text-3xl font-bold text-gray-900 dark:text-white mb-4">
        {blog.title}
      </h1>

      {blog.author_name && (
        <p className="text-sm text-gray-600 dark:text-gray-400 mb-1">
          By {blog.author_name}
        </p>
      )}

      {blog.published_at && (
        <p className="text-xs text-gray-500 dark:text-gray-400 mb-6">
          {new Date(blog.published_at).toLocaleDateString()}
        </p>
      )}

      {blog.content ? (
        <div
          className="prose dark:prose-invert max-w-none"
          dangerouslySetInnerHTML={{ __html: blog.content }}
        />
      ) : (
        <p className="text-gray-500">No content available.</p>
      )}
    </div>
  );
};

export default BlogDetailsPage;
