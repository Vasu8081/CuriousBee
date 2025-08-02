import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Navbar from "./components/Navbar";
import Footer from "./components/Footer";
import Home from "./pages/Home";
import Videos from "./pages/Videos";
import Resources from "./pages/Resources";
import Blog from "./pages/Blog";
import PrivacyPolicy from "./pages/PrivacyPolicy";
import TermsAndConditions from "./pages/TermsAndConditions";
import ContactPage from "./pages/Contact.tsx";
import BlogDetailsPage from "./pages/BlogDetailsPage";

if (!localStorage.getItem("theme")) {
  localStorage.setItem("theme", "dark");
}

export default function App() {
  return (
    <Router>
      <div className="min-h-screen flex flex-col">
        <Navbar />
        <main className="flex-grow">
          <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/videos" element={<Videos />} />
            <Route path="/resources" element={<Resources />} />
            <Route path="/blog" element={<Blog />} />
            <Route path="/privacy-policy" element={<PrivacyPolicy />} />
            <Route path="/terms-and-conditions" element={<TermsAndConditions />} />
            <Route path="/contact" element={<ContactPage />} />
            <Route path="/blog/:slug" element={<BlogDetailsPage />} />
          </Routes>
        </main>
        <Footer />
      </div>
    </Router>
  );
}
