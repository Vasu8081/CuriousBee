import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Navbar from "./components/Navbar";
import Footer from "./components/Footer";
import Home from "./pages/Home";
import Videos from "./pages/Videos";
import GatePrep from "./pages/GatePrep";
import Interviews from "./pages/Interviews";
import KeyPoints from "./pages/KeyPoints";
import Blog from "./pages/Blog";

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
            <Route path="/gate" element={<GatePrep />} />
            <Route path="/interviews" element={<Interviews />} />
            <Route path="/keypoints" element={<KeyPoints />} />
            <Route path="/blog" element={<Blog />} />
          </Routes>
        </main>
        <Footer />
      </div>
    </Router>
  );
}
