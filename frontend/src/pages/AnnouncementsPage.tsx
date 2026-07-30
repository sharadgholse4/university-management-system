import React, { useState } from 'react';
import { useAuth } from '../context/AuthContext';
import { Megaphone, Plus, Tag, CheckCircle2 } from 'lucide-react';
import { INITIAL_ANNOUNCEMENTS } from '../data/mockData';

export const AnnouncementsPage: React.FC = () => {
  const { user } = useAuth();
  const role = user?.role || 'student';

  const [announcements, setAnnouncements] = useState(INITIAL_ANNOUNCEMENTS);
  const [selectedCategory, setSelectedCategory] = useState<string>('all');
  const [showModal, setShowModal] = useState(false);

  const [title, setTitle] = useState('');
  const [content, setContent] = useState('');
  const [category, setCategory] = useState('Exam');
  const [msg, setMsg] = useState('');

  const handleCreate = (e: React.FormEvent) => {
    e.preventDefault();
    const newAnn = {
      id: `ann_${Date.now()}`,
      title,
      content,
      date: 'Just now',
      category,
      author: user?.role === 'admin' ? 'Academic Administration' : 'Faculty Member',
    };

    setAnnouncements((prev) => [newAnn, ...prev]);
    setMsg('Announcement broadcasted successfully!');
    setShowModal(false);
    setTitle('');
    setContent('');
    setTimeout(() => setMsg(''), 3000);
  };

  const filtered =
    selectedCategory === 'all'
      ? announcements
      : announcements.filter((a) => a.category.toLowerCase() === selectedCategory.toLowerCase());

  return (
    <div className="space-y-6 animate-fade-in">
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-900 tracking-tight">University Notice Board</h1>
          <p className="text-xs text-slate-500 mt-1">Official announcements, examination notices, and campus events</p>
        </div>

        {role !== 'student' && (
          <button
            onClick={() => setShowModal(true)}
            className="px-4 py-2 rounded-xl bg-amber-600 hover:bg-amber-500 text-white font-bold text-xs shadow-md shadow-amber-600/20 flex items-center gap-1.5 self-start sm:self-auto"
          >
            <Plus className="w-4 h-4" /> Post Notice
          </button>
        )}
      </div>

      {msg && (
        <div className="p-3 rounded-xl bg-emerald-50 border border-emerald-200 text-emerald-700 text-xs font-semibold flex items-center gap-2">
          <CheckCircle2 className="w-4 h-4" /> {msg}
        </div>
      )}

      {/* Category Filter Pills */}
      <div className="flex items-center gap-2 overflow-x-auto pb-1">
        {['all', 'Exam', 'Notice', 'Event'].map((cat) => (
          <button
            key={cat}
            onClick={() => setSelectedCategory(cat)}
            className={`px-3.5 py-1.5 rounded-xl text-xs font-bold transition-all capitalize ${
              selectedCategory === cat
                ? 'bg-amber-600 text-white shadow-sm'
                : 'bg-white text-slate-600 border border-slate-200 hover:bg-slate-50'
            }`}
          >
            {cat}
          </button>
        ))}
      </div>

      {/* Announcement List */}
      <div className="space-y-4">
        {filtered.map((ann) => (
          <div key={ann.id} className="glass-panel p-6 rounded-3xl border border-slate-200 shadow-sm hover:border-slate-300 transition-all">
            <div className="flex items-center justify-between mb-2">
              <div className="flex items-center gap-2">
                <span className="px-2.5 py-0.5 rounded-full text-[10px] font-extrabold uppercase tracking-wider bg-amber-100 text-amber-800 border border-amber-300">
                  {ann.category}
                </span>
                <span className="text-xs text-slate-400 font-medium">By {ann.author}</span>
              </div>
              <span className="text-xs text-slate-400 font-mono">{ann.date}</span>
            </div>

            <h3 className="text-base font-bold text-slate-900">{ann.title}</h3>
            <p className="text-xs text-slate-600 leading-relaxed mt-2">{ann.content}</p>
          </div>
        ))}
      </div>

      {/* Post Modal */}
      {showModal && (
        <div className="fixed inset-0 bg-slate-900/60 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-white rounded-3xl p-6 w-full max-w-md shadow-2xl border border-slate-200">
            <h3 className="text-lg font-bold text-slate-900 mb-4 flex items-center gap-2">
              <Megaphone className="w-5 h-5 text-amber-600" /> Post New Announcement
            </h3>
            <form onSubmit={handleCreate} className="space-y-4 text-xs">
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Title</label>
                <input
                  type="text"
                  required
                  placeholder="e.g. Campus Holiday Notice"
                  value={title}
                  onChange={(e) => setTitle(e.target.value)}
                  className="w-full px-3.5 py-2 rounded-xl bg-slate-50 border border-slate-200 text-sm"
                />
              </div>
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Category</label>
                <select
                  value={category}
                  onChange={(e) => setCategory(e.target.value)}
                  className="w-full px-3.5 py-2 rounded-xl bg-slate-50 border border-slate-200 text-sm"
                >
                  <option value="Exam">Exam</option>
                  <option value="Notice">Notice</option>
                  <option value="Event">Event</option>
                  <option value="General">General</option>
                </select>
              </div>
              <div>
                <label className="block font-semibold text-slate-600 mb-1">Notice Content</label>
                <textarea
                  required
                  rows={4}
                  placeholder="Write announcement details..."
                  value={content}
                  onChange={(e) => setContent(e.target.value)}
                  className="w-full px-3.5 py-2 rounded-xl bg-slate-50 border border-slate-200 text-sm"
                />
              </div>
              <div className="flex gap-2 pt-2">
                <button
                  type="button"
                  onClick={() => setShowModal(false)}
                  className="flex-1 py-2.5 rounded-xl bg-slate-100 hover:bg-slate-200 text-slate-700 font-bold"
                >
                  Cancel
                </button>
                <button
                  type="submit"
                  className="flex-1 py-2.5 rounded-xl bg-amber-600 hover:bg-amber-500 text-white font-bold shadow-md"
                >
                  Publish Notice
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </div>
  );
};
