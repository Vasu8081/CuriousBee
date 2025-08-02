import React, { useEffect, useState } from "react";
import { MdArticle } from "react-icons/md";
import { motion, AnimatePresence } from "framer-motion";
import { Link } from "react-router-dom";
import { semanticColors } from "../constants/theme";

interface Blog {
  id: string;
  title: string;
  slug?: string;
  author_name?: string;
  cover_image?: string;
  published_at?: string;
}

const BlogPage: React.FC = () => {
  const [blogs, setBlogs] = useState<Blog[]>([]);
  const [loading, setLoading] = useState<boolean>(false);

  const fetchBlogs = async (): Promise<void> => {
    setLoading(true);
    try {
      const res = await fetch("/api/blogs");
      const data: Blog[] = await res.json();
      setBlogs(data || []);
    } catch (err) {
      console.error("Failed to load blogs:", err);
      setBlogs([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchBlogs();
  }, []);

  return (
    <div className="px-4 py-10 max-w-7xl mx-auto">
      {/* Title */}
      <div className="flex justify-center items-center gap-3 mb-6">
        <MdArticle className="text-3xl text-indigo-600 dark:text-indigo-400" />
        <h2 className="text-3xl font-bold text-gray-900 dark:text-white">Blogs</h2>
      </div>

      {/* Content */}
      {loading ? (
        <p className="text-center text-gray-500">Loading blogs...</p>
      ) : blogs.length === 0 ? (
        <p className="text-center text-gray-500">No blogs present.</p>
      ) : (
        <motion.div
          className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-6"
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.4 }}
        >
          <AnimatePresence mode="wait">
            {blogs.map((blog) => (
              <motion.div
                key={blog.id}
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0 }}
                transition={{ duration: 0.3, delay: 0.05 }}
              >
                <Link to={`/blog/${blog.slug || blog.id}`}>
                  <div
                    className={`rounded-xl overflow-hidden border hover:scale-105 transition-transform duration-300 cursor-pointer 
                    ${semanticColors.cards.social} shadow-md`}
                  >
                    {blog.cover_image && (
                      <img
                        src={blog.cover_image}
                        alt={blog.title}
                        className="w-full h-48 object-cover"
                      />
                    )}
                    <div className="p-4">
                      <h3 className="text-lg font-bold text-gray-900 dark:text-white mb-1">
                        {blog.title}
                      </h3>
                      {blog.author_name && (
                        <p className="text-sm text-gray-600 dark:text-gray-400">
                          By {blog.author_name}
                        </p>
                      )}
                      {blog.published_at && (
                        <p className="text-xs text-gray-500 dark:text-gray-400 mt-1">
                          {new Date(blog.published_at).toLocaleDateString()}
                        </p>
                      )}
                    </div>
                  </div>
                </Link>
              </motion.div>
            ))}
          </AnimatePresence>
        </motion.div>
      )}
    </div>
  );
};

export default BlogPage;
