const char* HTML_CONTENT = R""""(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<meta name="mobile-web-app-capable" content="yes">
<title>Controller</title>
<style>
* { -webkit-tap-highlight-color: transparent; user-select: none; touch-action: none; -webkit-touch-callout: none; -webkit-user-select: none; box-sizing: border-box; }
body, html { margin: 0; padding: 0; overflow: hidden; width: 100%; height: 100%; background: #0a0a0a; }

#controller {
  position: fixed;
  inset: 0;
  width: 100%;
  height: 100%;
  background: #0a0a0a;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

#main-area {
  flex: 1;
  display: flex;
  position: relative;
  min-height: 0;
}

#left-zone, #right-zone {
  flex: 1;
  display: flex;
  flex-direction: column;
  position: relative;
}

#left-zone {
  border-right: 1px solid #1a1a1a;
}

.ctrl-btn {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  position: relative;
  transition: background 0.05s;
}

.ctrl-btn svg {
  width: 48px;
  height: 48px;
  pointer-events: none;
  opacity: 0.35;
  transition: opacity 0.05s;
}

.ctrl-btn.active {
  background: rgba(255,255,255,0.07) !important;
}

.ctrl-btn.active svg {
  opacity: 1;
}

#btn-fwd {
  border-bottom: 1px solid #1a1a1a;
}

#right-zone {
  flex-direction: row;
}

#btn-left {
  border-right: 1px solid #1a1a1a;
}

.corner-glow {
  position: absolute;
  inset: 0;
  pointer-events: none;
  border: 1px solid transparent;
  border-radius: 2px;
  transition: border-color 0.05s;
}

.ctrl-btn.active .corner-glow {
  border-color: rgba(255,255,255,0.12);
}

</style>
</head>
<body>

<div id="controller" role="application" aria-label="Пульт управления">
  <div id="main-area">
    <div id="left-zone">
      <div class="ctrl-btn" id="btn-fwd" data-action="fwd">
        <div class="corner-glow"></div>
        <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
          <polygon points="24,6 44,38 4,38" fill="white"/>
        </svg>
      </div>
      <div class="ctrl-btn" id="btn-bwd" data-action="bwd">
        <div class="corner-glow"></div>
        <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
          <polygon points="24,42 44,10 4,10" fill="white"/>
        </svg>
      </div>
    </div>
    <div id="right-zone">
      <div class="ctrl-btn" id="btn-left" data-action="left">
        <div class="corner-glow"></div>
        <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
          <polygon points="6,24 38,4 38,44" fill="white"/>
        </svg>
      </div>
      <div class="ctrl-btn" id="btn-right" data-action="right">
        <div class="corner-glow"></div>
        <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
          <polygon points="42,24 10,4 10,44" fill="white"/>
        </svg>
      </div>
    </div>
  </div>
</div>

<script> 
  const API = {
    GO_ON:     "/go/on",
    GO_OFF:    "/go/off",
    BACK_ON:   "/back/on",
    BACK_OFF:  "/back/off",
    LEFT_ON:   "/left/on",
    LEFT_OFF:  "/left/off",
    RIGHT_ON:  "/right/on",
    RIGHT_OFF: "/right/off"
  };

  const ACTION_ON = {
    fwd:   API.GO_ON,
    bwd:   API.BACK_ON,
    left:  API.LEFT_ON,
    right: API.RIGHT_ON
  };

  const ACTION_OFF = {
    fwd:   API.GO_OFF,
    bwd:   API.BACK_OFF,
    left:  API.LEFT_OFF,
    right: API.RIGHT_OFF
  };
  
  function makeTimeoutSignal(ms) {
    if (typeof AbortSignal.timeout === 'function') {
      return AbortSignal.timeout(ms);
    }
    const ctrl = new AbortController();
    setTimeout(() => ctrl.abort(), ms);
    return ctrl.signal;
  }
  
  async function sendCommand(url) {
    try {
      await fetch(url, {
        method: 'GET',
        cache: 'no-store',
        signal: makeTimeoutSignal(1500)
      });
    } catch (e) {}
  }
  
  const activeActions = new Set();
  const touchToAction = new Map();

  function getActionAt(x, y) {
    const el = document.elementFromPoint(x, y);
    if (!el) return null;
    const btn = el.closest('[data-action]');
    return btn ? btn.dataset.action : null;
  }

  function activate(action) {
    if (activeActions.has(action)) return;

    activeActions.add(action);
    const el = document.querySelector('[data-action="' + action + '"]');
    if (el) el.classList.add('active');
    if (navigator.vibrate) navigator.vibrate(10);
    sendCommand(ACTION_ON[action]);
  }

  function deactivate(action) {
    if (!activeActions.has(action)) return;
    for (const act of touchToAction.values()) {
      if (act === action) return;
    }
    activeActions.delete(action);
    const el = document.querySelector('[data-action="' + action + '"]');
    if (el) el.classList.remove('active');
    if (navigator.vibrate) navigator.vibrate(5);
    sendCommand(ACTION_OFF[action]);
  }
  
  function onTouchStart(e) {
    e.preventDefault();
    for (const t of e.changedTouches) {
      const action = getActionAt(t.clientX, t.clientY);
      if (action) {
        touchToAction.set(t.identifier, action);
        activate(action);
      }
    }
  }

  function onTouchMove(e) {
    e.preventDefault();
    for (const t of e.changedTouches) {
      const prev   = touchToAction.get(t.identifier);
      const newAct = getActionAt(t.clientX, t.clientY);
      if (prev === newAct) continue;
      if (prev) {
        touchToAction.delete(t.identifier);
        deactivate(prev);
      }
      if (newAct) {
        touchToAction.set(t.identifier, newAct);
        activate(newAct);
      }
    }
  }

  function onTouchEnd(e) {
    e.preventDefault();
    for (const t of e.changedTouches) {
      const action = touchToAction.get(t.identifier);
      if (action) {
        touchToAction.delete(t.identifier);
        deactivate(action);
      }
    }
  }

  const ctrl = document.getElementById('controller');
  ctrl.addEventListener('touchstart',  onTouchStart,  { passive: false });
  ctrl.addEventListener('touchmove',   onTouchMove,   { passive: false });
  ctrl.addEventListener('touchend',    onTouchEnd,    { passive: false });
  ctrl.addEventListener('touchcancel', onTouchEnd,    { passive: false });
  
  let mouseAction = null;

  ctrl.addEventListener('mousedown', e => {
    if (mouseAction) return;
    const btn = e.target.closest('[data-action]');
    if (btn) {
      mouseAction = btn.dataset.action;
      touchToAction.set('mouse', mouseAction);
      activate(mouseAction);
    }
  });

  document.addEventListener('mouseup', () => {
    if (mouseAction) {
      const prev = mouseAction;
      mouseAction = null;
      touchToAction.delete('mouse');
      deactivate(prev);
    }
  });
  
  document.addEventListener('touchmove',     e => e.preventDefault(), { passive: false });
  document.addEventListener('gesturestart',  e => e.preventDefault());
  document.addEventListener('gesturechange', e => e.preventDefault());
</script>
</body>
</html>
)"""";
