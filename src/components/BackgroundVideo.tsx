import React, { useState, useEffect, useRef } from 'react';

const VIDEO_BG_URL = 'https://d8j0ntlcm91z4.cloudfront.net/user_38xzZboKViGWJOttwIXH07lWA1P/hf_20260319_055001_8e16d972-3b2b-441c-86ad-2901a54682f9.mp4';

export const BackgroundVideo: React.FC = () => {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [videoError, setVideoError] = useState(false);

  useEffect(() => {
    if (videoRef.current && !videoError) {
      videoRef.current.muted = true;
      videoRef.current.play().catch(e => {
        console.warn('[Video BG] Autoplay policy prevented immediate playback:', e);
      });
    }
  }, [videoError]);

  return (
    <div className="fixed inset-0 w-full h-full pointer-events-none z-0 overflow-hidden bg-gradient-to-br from-slate-950 via-indigo-950 to-slate-950">
      {!videoError && (
        <video
          ref={videoRef}
          autoPlay
          loop
          muted
          playsInline
          preload="auto"
          onError={() => setVideoError(true)}
          className="w-full h-full object-cover opacity-40 scale-105"
          src={VIDEO_BG_URL}
        />
      )}
      <div className="absolute inset-0 bg-slate-950/60 backdrop-blur-[1px]" />
    </div>
  );
};
