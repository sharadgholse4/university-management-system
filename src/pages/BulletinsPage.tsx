import React, { useState, useContext } from 'react';
import { AuthContext } from '../context/AuthContext';
import { BulletinNotice } from '../types';

export const BulletinsPage: React.FC = () => {
  const auth = useContext(AuthContext);
  const user = auth?.user;

  const [bulletins, setBulletins] = useState<BulletinNotice[]>([
    {
      id: 1,
      title: 'Fall 2026 Mid-Semester Examination Schedule Published',
      category: 'Academic',
      date: '2026-08-01',
      author: 'Academic Affairs Office',
      content: 'The official schedule for mid-semester examinations has been published. All students must review their course dates.'
    },
    {
      id: 2,
      title: 'University Research Grant Call for Proposals',
      category: 'Research',
      date: '2026-07-28',
      author: 'Office of Research',
      content: 'Faculty members are invited to submit research funding proposals for the upcoming fiscal cycle.'
    }
  ]);

  const [showAddNoticeModal, setShowAddNoticeModal] = useState(false);
  const [noticeTitle, setNoticeTitle] = useState('');
  const [noticeCategory, setNoticeCategory] = useState<'Academic' | 'Examination' | 'Research' | 'Campus Event'>('Academic');
  const [noticeContent, setNoticeContent] = useState('');

  const handlePostNotice = (e: React.FormEvent) => {
    e.preventDefault();
    if (!noticeTitle || !noticeContent) return;

    const newNotice: BulletinNotice = {
      id: Date.now(),
      title: noticeTitle,
      category: noticeCategory,
      date: new Date().toISOString().split('T')[0],
      author: user?.name || 'Department Office',
      content: noticeContent
    };

    setBulletins([newNotice, ...bulletins]);
    setNoticeTitle('');
    setNoticeContent('');
    setShowAddNoticeModal(false);
  };

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h2 className="text-2xl font-black text-white">University Bulletins</h2>
          <p className="text-xs text-slate-400 font-medium">Official university circulars, academic notices, and institutional updates</p>
        </div>
        {(user?.role === 'professor' || user?.role === 'admin') && (
          <button
            onClick={() => setShowAddNoticeModal(true)}
            className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white font-bold rounded-xl text-xs shadow-md transition-colors"
          >
            + Publish Announcement
          </button>
        )}
      </div>

      <div className="space-y-4">
        {bulletins.map(notice => (
          <div key={notice.id} className="bg-slate-900 p-6 rounded-2xl border border-slate-800 shadow-sm space-y-3">
            <div className="flex items-center justify-between">
              <span className="px-3 py-1 rounded text-[11px] font-bold bg-indigo-500/20 text-indigo-300 border border-indigo-500/30">
                {notice.category}
              </span>
              <span className="text-xs text-slate-400 font-medium">{notice.date}</span>
            </div>
            <h3 className="text-lg font-bold text-white">{notice.title}</h3>
            <p className="text-xs text-slate-300 leading-relaxed font-medium">{notice.content}</p>
            <div className="text-[11px] text-slate-500 font-bold pt-2 border-t border-slate-800">{`Issued by: ${notice.author}`}</div>
          </div>
        ))}
      </div>

      {showAddNoticeModal && (
        <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="bg-slate-900 border border-slate-800 p-6 rounded-3xl max-w-md w-full space-y-4 shadow-2xl">
            <div className="flex justify-between items-center">
              <h3 className="text-lg font-bold text-white">Publish Announcement Bulletin</h3>
              <button onClick={() => setShowAddNoticeModal(false)} className="text-slate-400 hover:text-white font-bold">
                ✕
              </button>
            </div>
            <form onSubmit={handlePostNotice} className="space-y-3 text-xs">
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Title</label>
                <input
                  type="text"
                  value={noticeTitle}
                  onChange={(e) => setNoticeTitle(e.target.value)}
                  placeholder="Announcement title..."
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Category</label>
                <select
                  value={noticeCategory}
                  onChange={(e) => setNoticeCategory(e.target.value as any)}
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                >
                  <option value="Academic">Academic</option>
                  <option value="Examination">Examination</option>
                  <option value="Research">Research</option>
                  <option value="Campus Event">Campus Event</option>
                </select>
              </div>
              <div className="space-y-1">
                <label className="font-bold text-slate-300">Content</label>
                <textarea
                  rows={3}
                  value={noticeContent}
                  onChange={(e) => setNoticeContent(e.target.value)}
                  placeholder="Detailed announcement text..."
                  className="w-full px-3 py-2 bg-slate-800 border border-slate-700 text-white rounded-xl"
                />
              </div>
              <div className="flex justify-end gap-2 pt-2">
                <button type="button" onClick={() => setShowAddNoticeModal(false)} className="px-4 py-2 bg-slate-800 text-slate-300 rounded-xl font-bold">
                  Cancel
                </button>
                <button type="submit" className="px-4 py-2 bg-indigo-600 hover:bg-indigo-500 text-white rounded-xl font-bold">
                  Publish
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </div>
  );
};
