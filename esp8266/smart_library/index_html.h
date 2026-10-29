const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Library</title>
<style>
:root {
  --text: #403C3C;
  --muted: rgba(64,60,60,.7);
  --accent: #829AA6;
  --beige: rgba(235,225,216,.7);
  --gray: #635D5D;
  --pink: #E7C4BE;
  --shadow: 0 4px 10px rgba(0,0,0,.25);
}
* { box-sizing: border-box; margin: 0; padding: 0 }
body {
  font-family: "Arial Rounded MT Bold", "Nunito", "Varela Round", system-ui, sans-serif;
  color: var(--text); background: #fff;
  height: 100vh; height: 100dvh; display: flex; overflow: hidden;
}
button, input { font: inherit; color: inherit; border: 0; background: none; outline: 0 }
button { cursor: pointer }
.icon { fill: none; stroke: currentColor; stroke-width: 2; stroke-linecap: round; stroke-linejoin: round }

/* Sidebar */
.sidebar {
  position: relative; width: 96px; flex-shrink: 0; background: #fff; z-index: 3;
  box-shadow: 0 4px 20px rgba(0,0,0,.12); transition: width .3s;
}
.menu-title { text-align: center; font-size: 20px; margin: 36px 0 30px; transition: font-size .3s, margin .3s }
.nav { display: flex; flex-direction: column; gap: 14px; overflow: hidden; transition: gap .3s }
.nav a {
  display: flex; align-items: center; gap: 10px; height: 44px; padding-left: 30px;
  color: var(--muted); text-decoration: none; font-size: 17px; white-space: nowrap;
  transition: padding .3s, height .3s;
}
.nav a:hover, .nav a.active { color: var(--text) }
.nav .icon { width: 36px; height: 36px; flex-shrink: 0; color: var(--text); transition: width .3s, height .3s }
.nav span { opacity: 0; transition: opacity .15s }
.subnav { display: none; margin-left: 30px; border-left: 3px solid #D9D9D9; padding-left: 6px }
.sidebar-toggle {
  position: absolute; top: 84px; right: -16px; width: 32px; height: 32px;
  border-radius: 8px; background: #FBF9F7; box-shadow: 0 2px 4px rgba(0,0,0,.2);
  display: grid; place-items: center;
}
.sidebar-toggle .icon { width: 16px; height: 16px; transition: transform .3s }

.menu-open .sidebar { width: 220px; box-shadow: var(--shadow) }
.menu-open .menu-title { font-size: 28px; margin: 28px 0 22px }
.menu-open .nav { gap: 4px }
.menu-open .nav a { height: 40px; padding-left: 22px }
.menu-open .nav .icon { width: 28px; height: 28px }
.menu-open .nav span { opacity: 1; transition: opacity .2s .15s }
.menu-open .subnav a { padding-left: 0; font-size: 16px }
.menu-open .subnav .icon { width: 24px; height: 24px }
.menu-open .sidebar-toggle .icon { transform: rotate(180deg) }
.menu-open[data-page="env"] #subnav-env,
.menu-open[data-page="book"] #subnav-book { display: block }

/* Pages */
main { flex: 1; min-width: 0; display: flex; overflow: auto; padding: 32px 48px }
.page { display: none; width: 100%; margin: auto }
[data-page="home"] #page-home,
[data-page="env"] #page-env,
[data-page="book"] #page-book { display: block }
.title { font-size: clamp(40px, 7vw, 88px); font-weight: 400; text-align: center }

.cards { display: grid; grid-template-columns: repeat(2, 260px); justify-content: center; gap: 36px 44px }
.card {
  height: 250px; border-radius: 32px; background: #fff;
  box-shadow: 0 4px 20px 6px rgba(130,154,166,.15);
  display: flex; flex-direction: column; align-items: center; justify-content: center; gap: 10px;
}
.cards.tall .card { height: 320px; color: var(--gray) }
#page-env .card:last-child { grid-column: 1 / -1; justify-self: center; width: 260px }
.card > .icon { width: 84px; height: 84px; color: var(--accent) }
.card h2 { font-size: 26px; font-weight: 400; margin-bottom: 12px }
.manage {
  width: 132px; padding: 9px 0; font-size: 19px; color: var(--gray);
  background: var(--beige); border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,.2);
}
.manage:hover { background: rgba(235,225,216,.4) }
.manage:active, .box:active { transform: translateY(2px) }

/* Modal */
.overlay {
  position: fixed; inset: 0; z-index: 9; padding: 20px;
  background: rgba(217,217,217,.25);
  display: none; align-items: center; justify-content: center;
}
.overlay.show { display: flex }
.modal {
  display: none; width: 560px; max-width: 100%; max-height: 100%; overflow: auto;
  background: #fff; border-radius: 18px; box-shadow: var(--shadow);
  padding: 18px 28px 32px; color: var(--gray);
}
.modal.show { display: block }
.modal-head { display: flex; justify-content: space-between; align-items: center }
.modal-head h3 { font-size: 30px; font-weight: 400; padding: 4px }
.close { padding: 6px }
.close .icon { width: 22px; height: 22px; stroke-width: 2.5 }
.modal-body { display: flex; flex-direction: column; align-items: center; gap: 24px; margin-top: 6px }
.modal-body.loose { gap: 30px }
.modal-body.compact { gap: 18px }
.hero { width: 110px; height: 110px; color: var(--accent); flex-shrink: 0 }

.rows { display: flex; flex-direction: column; gap: 20px }
.row { display: flex; align-items: center; gap: 18px }
.row.wide { gap: 28px }
.row.tight { gap: 14px }
.pill {
  min-width: 110px; height: 40px; padding: 0 20px; border-radius: 20px;
  background: var(--beige); display: grid; place-items: center; font-size: 20px; white-space: nowrap;
}
.box {
  width: 112px; height: 40px; border-radius: 8px; background: #fff; box-shadow: var(--shadow);
  font-size: 19px; color: #5D5D5D; display: flex; align-items: center; justify-content: center;
  transition: background .3s;
}
.box:disabled { cursor: not-allowed; opacity: .55 }
.lock { width: 20px; height: 20px; color: var(--pink) }

/* Light: dropdown */
.dropdown-wrap { position: relative; width: 112px; height: 40px }
.dropdown {
  position: absolute; top: 0; left: 0; z-index: 2; width: 112px; height: 40px; overflow: hidden;
  background: #fff; border-radius: 8px; box-shadow: var(--shadow); transition: height .2s;
}
.dropdown.open { height: 80px }
.dropdown button {
  display: flex; align-items: center; justify-content: space-between;
  width: 100%; height: 40px; padding: 0 8px 0 12px; font-size: 19px; color: #5D5D5D;
}
.dropdown .other { border-top: 2px solid #E8E8E8; justify-content: center }
.dropdown .other:hover { background: var(--beige) }
.dropdown .icon { width: 16px; height: 16px; stroke-width: 2.4; transition: transform .2s }
.dropdown.open .icon { transform: rotate(180deg) }
#modal-light .rows { padding-bottom: 40px }   /* ที่ให้ dropdown กางลงโดยไม่โดนขอบหน้าต่างตัด */

/* Temperature */
.segment {
  display: flex; width: 132px; height: 44px; padding: 4px; gap: 4px;
  border-radius: 8px; background: #fff; box-shadow: var(--shadow);
}
.segment button { flex: 1; border-radius: 6px; font-size: 19px; color: #5D5D5D }
.segment .active { background: var(--beige) }
.temp-value { width: 120px }

/* Smoke */
#smoke-status { width: 124px; color: var(--gray) }
#smoke-status.danger { background: var(--pink) }

/* Seat */
.seats {
  display: flex; gap: 6px; width: 460px; max-width: 100%; padding: 10px;
  background: #fff; border-radius: 16px; box-shadow: var(--shadow);
}
.seat {
  flex: 1; min-width: 0; display: flex; flex-direction: column; align-items: center; gap: 4px;
  padding: 10px 0 8px; border-radius: 10px; background: var(--beige); transition: background .3s;
}
.seat b { font-weight: 400; font-size: 16px; color: var(--gray) }
.seat .icon { width: 30px; height: 30px; color: var(--pink); transition: color .3s }
.seat.taken { background: var(--pink) }
.seat.taken b, .seat.taken .icon { color: #fff }
.seat-summary { display: flex; justify-content: space-between; width: 460px; max-width: 100%; padding: 0 4px; font-size: 15px; color: rgba(99,93,93,.8) }
.seat-summary b { font-weight: 400; color: var(--gray) }

/* Search */
.search { position: relative; width: 400px; max-width: 100%; transition: width .3s }
.search-bar {
  display: flex; align-items: center; gap: 10px; height: 50px; padding: 0 20px 0 14px;
  border-radius: 100px; background: #fff; box-shadow: var(--shadow);
}
.search-bar .icon { display: block; width: 24px; height: 24px; color: rgba(99,93,93,.8) }
.search-bar input { flex: 1; min-width: 0; font-size: 19px; color: rgba(0,0,0,.6) }
.search-bar input::placeholder { color: rgba(99,93,93,.7) }
/* หน้าหนังสือแสดงทุกเล่มตั้งแต่เปิด พิมพ์ค้นหาแล้วกรองเล่มที่ไม่ตรงออก */
#modal-search .search { width: 100% }
#modal-search .search-bar { flex-direction: row-reverse; padding: 0 20px 0 22px }
.found { display: block; width: 100%; margin: -6px 0 -10px; font-size: 15px; color: rgba(0,0,0,.8) }
.results { display: grid; grid-template-columns: repeat(auto-fill, minmax(220px, 1fr)); gap: 14px; width: 100% }
.no-result { grid-column: 1 / -1; padding: 14px 0; text-align: center; font-size: 16px; color: rgba(99,93,93,.6) }

.book {
  position: relative; padding: 18px 16px 6px 20px;
  border-radius: 16px; background: #fff; box-shadow: var(--shadow);
}
.book h4 { font-size: 17px; font-weight: 400; color: var(--gray); padding-right: 78px }
.book p { font-size: 12px; color: rgba(0,0,0,.4); margin: 4px 0 14px }
.follow {
  position: absolute; top: 16px; right: 14px; width: 70px; height: 28px;
  border-radius: 50px; background: #fff; box-shadow: var(--shadow); font-size: 12px; color: #5D5D5D;
}
.location { display: flex; align-items: center; gap: 12px; margin: 0 0 12px 4px }
.location span {
  width: 56px; height: 24px; border-radius: 14px; background: var(--beige);
  display: grid; place-items: center; font-size: 13px; color: var(--gray);
}
.location b { font-weight: 400; font-size: 15px; min-width: 52px; color: var(--gray) }
.location i {
  display: block; width: 18px; height: 18px; border-radius: 50%;
  background: #fff; box-shadow: 0 2px 4px rgba(0,0,0,.25); transition: background .3s;
}
.book.followed .follow { background: #EAF1F5; color: var(--gray) }
.book.followed .location i { background: var(--color) }

/* Smoke alert: บังทุกหน้าเมื่อควันอันตราย ค้างจนกว่าควันหายและกด OK */
.alarm {
  position: fixed; inset: 0; z-index: 30; padding: 20px;
  display: none; align-items: center; justify-content: center;
  background: rgba(231,196,190,.55); backdrop-filter: blur(6px); -webkit-backdrop-filter: blur(6px);
}
.alarm.show { display: flex }
.alarm-card {
  width: 440px; max-width: 100%; padding: 30px 32px 32px; border-radius: 22px;
  background: #fff; box-shadow: var(--shadow); color: var(--gray);
  display: flex; flex-direction: column; align-items: center; gap: 18px; text-align: center;
}
.alarm.danger .alarm-card { animation: alarm-pulse 1.2s ease-in-out infinite }
.alarm-card > .icon { width: 120px; height: 120px; color: #D98E84 }
.alarm.cleared .alarm-card > .icon { color: var(--accent) }
.alarm-card h3 { font-size: 34px; font-weight: 400; color: var(--text) }
.alarm-card .row { justify-content: center }
.alarm-status { width: 124px }
.alarm.danger .alarm-status { background: var(--pink) }
.alarm-time { display: flex; flex-direction: column; gap: 6px; font-size: 15px; color: rgba(99,93,93,.85) }
.alarm-time b { font-weight: 400; font-size: 22px; color: var(--text) }
.alarm-time small { font-size: 13px; color: rgba(99,93,93,.7) }
.alarm-ok { display: none }
.alarm-sound {
  padding: 8px 18px; border-radius: 8px; background: var(--beige); font-size: 15px; color: var(--gray);
  box-shadow: 0 2px 4px rgba(0,0,0,.2);
}
.alarm.cleared .alarm-ok { display: block }
@keyframes alarm-pulse {
  50% { box-shadow: 0 4px 10px rgba(0,0,0,.25), 0 0 0 10px rgba(231,196,190,.6) }
}
@media (prefers-reduced-motion: reduce) {
  .alarm.danger .alarm-card { animation: none }
}

/* แท็บเล็ต: การ์ดคอลัมน์เดียว */
@media (max-width: 760px) {
  main { padding: 28px 20px 28px 36px }
  .cards { grid-template-columns: minmax(0, 300px); gap: 24px }
  #page-env .card:last-child { width: auto; justify-self: stretch }
  .card, .cards.tall .card { height: 220px }
}

/* มือถือ: เมนูเลื่อนออกจากซ้าย เนื้อหาเต็มจอ */
@media (max-width: 600px) {
  .sidebar {
    position: fixed; top: 0; bottom: 0; left: 0; width: 220px;
    transform: translateX(-100%); transition: transform .3s, box-shadow .3s; box-shadow: none;
  }
  .menu-open .sidebar { width: 220px; transform: none; box-shadow: var(--shadow) }
  .menu-title, .menu-open .menu-title { font-size: 26px; margin: 24px 0 18px }
  .nav a, .menu-open .nav a { height: 42px; padding-left: 22px }
  .nav .icon { width: 28px; height: 28px }
  .nav span { opacity: 1 }
  .sidebar-toggle { top: 12px; right: -46px; width: 34px; height: 34px }
  .menu-open .sidebar-toggle { right: -17px }

  main { padding: 60px 16px 20px }
  .title { font-size: clamp(36px, 12vw, 56px) }
  .cards { grid-template-columns: minmax(0, 280px); gap: 16px }
  .card, .cards.tall .card { height: auto; padding: 22px 12px; border-radius: 24px }
  .card > .icon { width: 60px; height: 60px }
  .card h2 { font-size: 22px; margin-bottom: 6px }
  .manage { width: 116px; padding: 8px 0; font-size: 17px }

  .overlay { padding: 10px }
  .modal { padding: 12px 16px 24px; border-radius: 16px }
  .modal-head h3 { font-size: 26px }
  .hero { width: 90px; height: 90px }
  .row, .row.wide, .row.tight { gap: 12px; flex-wrap: wrap; justify-content: center }
  .seats { gap: 4px; padding: 8px }
  .seat { padding: 8px 0 6px; border-radius: 8px }
  .seat b { font-size: 14px }
  .seat .icon { width: 22px; height: 22px }
  .seat-summary { font-size: 14px }
  .results { grid-template-columns: 1fr }

  .alarm-card { padding: 24px 18px 26px; gap: 14px }
  .alarm-card > .icon { width: 90px; height: 90px }
  .alarm-card h3 { font-size: 28px }
}
@media (max-width: 360px) {
  .pill { min-width: 0; padding: 0 14px; font-size: 18px }
  .box, .dropdown-wrap, .dropdown { width: 100px }
  .alarm-card h3 { font-size: 24px }
}
</style>
</head>
<body data-page="home">

<svg width="0" height="0" style="position:absolute">
 <symbol id="i-env" viewBox="0 0 68 62"><path fill="currentColor" stroke="none" d="M32.5834 17.4375C32.2056 17.4375 31.875 17.3083 31.5917 17.05C31.3084 16.7917 31.1667 16.4903 31.1667 16.1458V10.9792C31.1667 9.17084 31.8514 7.64237 33.2209 6.39376C34.5903 5.14515 36.2667 4.52084 38.25 4.52084H43.9167C44.2945 4.52084 44.625 4.65001 44.9084 4.90834C45.1917 5.16668 45.3334 5.46807 45.3334 5.81251V10.9792C45.3334 12.7875 44.6486 14.316 43.2792 15.5646C41.9097 16.8132 40.2334 17.4375 38.25 17.4375H32.5834ZM15.5834 32.9375C12.8445 32.9375 10.507 32.0549 8.57085 30.2896C6.63474 28.5243 5.66669 26.3931 5.66669 23.8958V17.4375C5.66669 16.7056 5.93869 16.0925 6.48269 15.5982C7.02669 15.1039 7.69913 14.8559 8.50002 14.8542H15.5834C18.3222 14.8542 20.6597 15.7368 22.5959 17.5021C24.532 19.2674 25.5 21.3986 25.5 23.8958V30.3542C25.5 31.0861 25.228 31.7001 24.684 32.1961C24.14 32.6921 23.4676 32.9392 22.6667 32.9375H15.5834ZM45.3334 56.1875C43.4917 56.1875 41.7331 55.9292 40.0577 55.4125C38.3822 54.8958 36.8589 54.1854 35.4875 53.2813L33.15 55.4125C32.6306 55.8861 31.9695 56.1229 31.1667 56.1229C30.3639 56.1229 29.7028 55.8861 29.1834 55.4125C28.6639 54.9389 28.4042 54.3361 28.4042 53.6042C28.4042 52.8722 28.6639 52.2695 29.1834 51.7958L31.5209 49.6646C30.5292 48.416 29.75 47.0279 29.1834 45.5003C28.6167 43.9726 28.3334 42.3684 28.3334 40.6875C28.3334 36.382 29.9861 32.7119 33.2917 29.6773C36.5972 26.6428 40.6111 25.1246 45.3334 25.1229H56.7375C58.2959 25.1229 59.6304 25.6293 60.741 26.6419C61.8517 27.6546 62.4061 28.8705 62.4042 30.2896V40.6875C62.4042 44.9931 60.7401 48.6528 57.4119 51.6667C54.0836 54.6806 50.0575 56.1875 45.3334 56.1875ZM45.3334 51.0208C48.4972 51.0208 51.1653 50.009 53.3375 47.9854C55.5097 45.9618 56.5959 43.5292 56.5959 40.6875V30.3542H45.3334C42.2167 30.3542 39.5486 31.3556 37.3292 33.3586C35.1097 35.3615 34 37.8045 34 40.6875C34 41.6778 34.1304 42.6147 34.391 43.4982C34.6517 44.3817 35.0408 45.2101 35.5584 45.9833L43.35 38.8792C43.8695 38.4056 44.5306 38.1688 45.3334 38.1688C46.1361 38.1688 46.7972 38.4056 47.3167 38.8792C47.8361 39.3528 48.0959 39.9556 48.0959 40.6875C48.0959 41.4195 47.8361 42.0222 47.3167 42.4958L39.525 49.6C40.375 50.0736 41.2845 50.4293 42.2535 50.6669C43.2225 50.9046 44.2491 51.0226 45.3334 51.0208Z"/></symbol>
 <symbol id="i-book" viewBox="0 0 69 69"><path fill="currentColor" stroke="none" fill-rule="evenodd" clip-rule="evenodd" d="M10.7813 23C10.7812 19.3781 12.22 15.9046 14.7811 13.3436C17.3421 10.7825 20.8156 9.34375 24.4375 9.34375H53.1875C55.9647 9.34375 58.2188 11.5977 58.2188 14.375V57.5C58.2188 58.8344 57.6887 60.1141 56.7451 61.0576C55.8016 62.0012 54.5219 62.5312 53.1875 62.5312H21.5625C18.7031 62.5312 15.9609 61.3954 13.939 59.3735C11.9171 57.3516 10.7813 54.6094 10.7813 51.75V23ZM53.9062 14.375V40.9687H21.5625C19.136 40.9687 16.8964 41.7709 15.0938 43.125V23C15.0938 20.5219 16.0782 18.1453 17.8305 16.393C19.5828 14.6407 21.9594 13.6562 24.4375 13.6562H33.6605C32.9028 19.3575 32.9716 25.1383 33.8646 30.82L34.0457 31.9614C34.1088 32.3616 34.2833 32.736 34.5493 33.0416C34.8153 33.3472 35.162 33.5717 35.5497 33.6893C35.9374 33.8069 36.3504 33.8129 36.7414 33.7067C37.1323 33.6004 37.4855 33.3861 37.7603 33.0884L41.6875 28.8334L45.6147 33.0884C45.8895 33.3861 46.2427 33.6004 46.6336 33.7067C47.0246 33.8129 47.4376 33.8069 47.8253 33.6893C48.213 33.5717 48.5597 33.3472 48.8257 33.0416C49.0917 32.736 49.2662 32.3616 49.3293 31.9614L49.5104 30.82C50.4035 25.1383 50.4722 19.3575 49.7145 13.6562H53.1875C53.3781 13.6562 53.5609 13.732 53.6957 13.8668C53.8305 14.0016 53.9062 14.1844 53.9062 14.375ZM45.3617 13.6562H38.0133C37.3896 18.0081 37.2854 22.4186 37.7028 26.795L39.5744 24.7652C39.8436 24.4734 40.1703 24.2405 40.534 24.0812C40.8977 23.9219 41.2905 23.8397 41.6875 23.8397C42.0845 23.8397 42.4773 23.9219 42.841 24.0812C43.2047 24.2405 43.5314 24.4734 43.8006 24.7652L45.6722 26.795C46.0896 22.4186 45.9854 18.0081 45.3617 13.6562ZM21.5625 45.2812H53.9062V57.5C53.9062 57.6906 53.8305 57.8734 53.6957 58.0082C53.5609 58.143 53.3781 58.2187 53.1875 58.2187H21.5625C19.8469 58.2187 18.2015 57.5372 16.9884 56.3241C15.7753 55.111 15.0938 53.4656 15.0938 51.75C15.0938 50.0344 15.7753 48.389 16.9884 47.1759C18.2015 45.9628 19.8469 45.2812 21.5625 45.2812Z"/></symbol>
 <symbol id="i-bulb" viewBox="-22.5 -11.25 135 135"><path fill="currentColor" stroke="none" d="M16.875 80.1V101.25C16.875 107.438 21.9375 112.5 28.125 112.5H61.875C68.0625 112.5 73.125 107.438 73.125 101.25V80.1C83.6438 71.6625 90 58.6688 90 45C90 20.1938 69.8063 0 45 0C20.1938 0 0 20.1938 0 45C0 58.6688 6.35625 71.6625 16.875 80.1ZM45 11.25C63.6188 11.25 78.75 26.3813 78.75 45C78.75 56.475 73.125 66.8812 63.6188 73.125H50.625V66.7125C60.3 64.1813 67.5 55.4625 67.5 45H56.25C56.25 51.1875 51.1875 56.25 45 56.25C38.8125 56.25 33.75 51.1875 33.75 45H22.5C22.5 55.4625 29.7 64.1813 39.375 66.7125V73.125H26.3812C21.7081 70.0766 17.8743 65.9046 15.2309 60.9911C12.5874 56.0776 11.2186 50.5794 11.25 45C11.25 26.3813 26.3813 11.25 45 11.25Z"/></symbol>
 <symbol id="i-bulbon" viewBox="0 0 200 200"><path fill="currentColor" stroke="none" d="M56.1663 133.333H91.6663V115.5C77.333 111.75 66.6663 98.8334 66.6663 83.3334H83.333C83.333 92.5 90.833 100 99.9997 100C109.166 100 116.666 92.5 116.666 83.3334H133.333C133.333 98.8334 122.666 111.75 108.333 115.5V133.333H143.833C158.083 120.833 166.666 102.583 166.666 83.3334C166.666 46.5834 136.75 16.6667 99.9997 16.6667C63.2497 16.6667 33.333 46.5834 33.333 83.3334C33.333 102.583 41.9163 120.833 56.1663 133.333ZM58.333 150V166.667C58.333 175.833 65.833 183.333 74.9997 183.333H125C134.166 183.333 141.666 175.833 141.666 166.667V150H58.333Z"/></symbol>
 <symbol id="i-temp" viewBox="0 0 135 135"><path fill="currentColor" stroke="none" d="M45 28.125C45 22.1576 47.3705 16.4347 51.5901 12.2151C55.8097 7.99553 61.5326 5.625 67.5 5.625C73.4674 5.625 79.1903 7.99553 83.4099 12.2151C87.6295 16.4347 90 22.1576 90 28.125V57.6844C96.8793 62.4745 102.05 69.3351 104.76 77.2676C107.471 85.2002 107.578 93.7906 105.068 101.789C102.558 109.787 97.5606 116.775 90.8036 121.736C84.0467 126.697 75.8827 129.373 67.5 129.373C59.1173 129.373 50.9533 126.697 44.1964 121.736C37.4394 116.775 32.4422 109.787 29.9319 101.789C27.4217 93.7906 27.5294 85.2002 30.2396 77.2676C32.9497 69.3351 38.1207 62.4745 45 57.6844V28.125ZM51.435 66.9094C46.5197 70.3299 42.8245 75.23 40.8873 80.8963C38.9501 86.5627 38.872 92.6994 40.6645 98.4132C42.4569 104.127 46.0263 109.119 50.853 112.664C55.6797 116.208 61.5117 118.12 67.5 118.12C73.4883 118.12 79.3203 116.208 84.147 112.664C88.9737 109.119 92.5431 104.127 94.3355 98.4132C96.128 92.6994 96.0499 86.5627 94.1127 80.8963C92.1755 75.23 88.4803 70.3299 83.565 66.9094L78.75 63.5513V28.125C78.75 25.1413 77.5647 22.2798 75.4549 20.17C73.3452 18.0603 70.4837 16.875 67.5 16.875C64.5163 16.875 61.6548 18.0603 59.5451 20.17C57.4353 22.2798 56.25 25.1413 56.25 28.125V63.5513L51.435 66.9094ZM45 90H90C90 95.9674 87.6295 101.69 83.4099 105.91C79.1903 110.129 73.4674 112.5 67.5 112.5C61.5326 112.5 55.8097 110.129 51.5901 105.91C47.3705 101.69 45 95.9674 45 90Z"/></symbol>
 <symbol id="i-smoke" viewBox="0 0 135 135"><path fill="currentColor" stroke="none" d="M67.7812 123.75C66.2812 123.281 65.1806 122.366 64.4794 121.005C63.7781 119.644 63.66 118.215 64.125 116.719C64.9687 114 65.3681 111.469 65.3231 109.125C65.2781 106.781 64.5037 104.344 63 101.812C60.1875 97.5 58.6406 93.4462 58.3594 89.6512C58.0781 85.8562 58.6875 81.5662 60.1875 76.7812C60.6562 75.2812 61.5712 74.1806 62.9325 73.4794C64.2937 72.7781 65.7225 72.66 67.2188 73.125C68.715 73.59 69.8175 74.505 70.5262 75.87C71.235 77.235 71.3513 78.6637 70.875 80.1562C70.0313 82.875 69.5869 85.3837 69.5419 87.6825C69.4969 89.9812 70.3162 92.4412 72 95.0625C74.625 99.2812 76.1025 103.337 76.4325 107.229C76.7625 111.122 76.2225 115.41 74.8125 120.094C74.3438 121.594 73.4288 122.696 72.0675 123.401C70.7063 124.106 69.2775 124.222 67.7812 123.75ZM43.0312 123.75C41.5312 123.281 40.4306 122.366 39.7294 121.005C39.0281 119.644 38.91 118.215 39.375 116.719C40.125 114.094 40.5244 111.562 40.5731 109.125C40.6219 106.687 39.8475 104.25 38.25 101.812C35.4375 97.6875 33.915 93.6337 33.6825 89.6512C33.45 85.6687 34.035 81.3787 35.4375 76.7812C35.9063 75.2812 36.8213 74.1806 38.1825 73.4794C39.5438 72.7781 40.9725 72.66 42.4688 73.125C43.965 73.59 45.0675 74.505 45.7762 75.87C46.485 77.235 46.6012 78.6637 46.125 80.1562C45.375 82.7812 44.9531 85.3125 44.8594 87.75C44.7656 90.1875 45.5625 92.625 47.25 95.0625C49.9687 99.1875 51.4688 103.266 51.75 107.297C52.0312 111.328 51.4687 115.594 50.0625 120.094C49.5937 121.594 48.6806 122.694 47.3231 123.396C45.9656 124.097 44.535 124.215 43.0312 123.75ZM91.9688 123.75C90.4688 123.281 89.3681 122.366 88.6669 121.005C87.9656 119.644 87.8475 118.215 88.3125 116.719C89.0625 114.094 89.4619 111.562 89.5106 109.125C89.5594 106.687 88.785 104.25 87.1875 101.812C84.375 97.6875 82.8525 93.6337 82.62 89.6512C82.3875 85.6687 82.9725 81.3787 84.375 76.7812C84.8438 75.2812 85.7588 74.1806 87.12 73.4794C88.4812 72.7781 89.91 72.66 91.4062 73.125C92.9025 73.59 94.005 74.505 94.7137 75.87C95.4225 77.235 95.5387 78.6637 95.0625 80.1562C94.2188 82.875 93.7744 85.4062 93.7294 87.75C93.6844 90.0937 94.5038 92.5312 96.1875 95.0625C98.8125 99.1875 100.29 103.266 100.62 107.297C100.95 111.328 100.41 115.594 99 120.094C98.5312 121.594 97.6162 122.696 96.255 123.401C94.8937 124.106 93.465 124.222 91.9688 123.75ZM28.125 28.125V33.75H106.875V28.125H28.125ZM45.5625 45L47.25 50.625H87.75L89.4375 45H45.5625ZM47.25 61.875C44.8125 61.875 42.6094 61.1494 40.6406 59.6981C38.6719 58.2469 37.3125 56.3475 36.5625 54L33.75 45H28.125C25.0312 45 22.3838 43.8994 20.1825 41.6981C17.9813 39.4969 16.8787 36.8475 16.875 33.75V28.125C16.875 25.0312 17.9775 22.3837 20.1825 20.1825C22.3875 17.9813 25.035 16.8787 28.125 16.875H106.875C109.969 16.875 112.618 17.9775 114.823 20.1825C117.028 22.3875 118.129 25.035 118.125 28.125V33.75C118.125 36.8437 117.024 39.4931 114.823 41.6981C112.622 43.9031 109.973 45.0037 106.875 45H101.25L97.5938 54.5625C96.75 56.7187 95.3906 58.4775 93.5156 59.8387C91.6406 61.2 89.5312 61.8787 87.1875 61.875H47.25Z"/></symbol>
 <symbol id="i-smoke2" viewBox="-25 -25 200 200"><path fill="currentColor" stroke="none" d="M75.0396 157.542C72.8285 156.851 71.2061 155.502 70.1724 153.495C69.1387 151.489 68.9646 149.383 69.65 147.177C70.8938 143.169 71.4825 139.438 71.4161 135.983C71.3498 132.528 70.2083 128.935 67.9917 125.204C63.8458 118.847 61.5656 112.872 61.151 107.278C60.7365 101.683 61.6347 95.3597 63.8458 88.3063C64.5368 86.0951 65.8856 84.4727 67.8922 83.439C69.8988 82.4053 72.0048 82.2312 74.2104 82.9167C76.416 83.6021 78.0412 84.9509 79.0859 86.963C80.1307 88.9751 80.302 91.0812 79.6 93.2812C78.3563 97.2889 77.7012 100.987 77.6349 104.376C77.5685 107.764 78.7764 111.39 81.2583 115.254C85.1278 121.473 87.3057 127.451 87.7922 133.189C88.2786 138.927 87.4826 145.248 85.4042 152.152C84.7132 154.363 83.3644 155.988 81.3578 157.028C79.3513 158.067 77.2452 158.238 75.0396 157.542ZM38.5563 157.542C36.3451 156.851 34.7227 155.502 33.689 153.495C32.6553 151.489 32.4812 149.383 33.1667 147.177C34.2722 143.308 34.8609 139.576 34.9328 135.983C35.0047 132.39 33.8632 128.797 31.5083 125.204C27.3625 119.124 25.1182 113.148 24.7755 107.278C24.4328 101.407 25.2951 95.0833 27.3625 88.3063C28.0535 86.0951 29.4023 84.4727 31.4088 83.439C33.4154 82.4053 35.5215 82.2312 37.7271 82.9167C39.9327 83.6021 41.5578 84.9509 42.6026 86.963C43.6473 88.9751 43.8187 91.0812 43.1167 93.2812C42.0111 97.1507 41.3892 100.882 41.251 104.475C41.1128 108.068 42.2875 111.661 44.775 115.254C48.7826 121.335 50.9938 127.346 51.4083 133.289C51.8229 139.231 50.9937 145.519 48.9208 152.152C48.2299 154.363 46.8839 155.986 44.8828 157.019C42.8817 158.053 40.7729 158.227 38.5563 157.542ZM110.694 157.542C108.483 156.851 106.86 155.502 105.827 153.495C104.793 151.489 104.619 149.383 105.304 147.177C106.41 143.308 106.998 139.576 107.07 135.983C107.142 132.39 106.001 128.797 103.646 125.204C99.5 119.124 97.2557 113.148 96.913 107.278C96.5703 101.407 97.4326 95.0833 99.5 88.3063C100.191 86.0951 101.54 84.4727 103.546 83.439C105.553 82.4053 107.659 82.2312 109.865 82.9167C112.07 83.6021 113.695 84.9509 114.74 86.963C115.785 88.9751 115.956 91.0812 115.254 93.2812C114.01 97.2889 113.355 101.02 113.289 104.475C113.223 107.93 114.431 111.523 116.913 115.254C120.782 121.335 122.96 127.346 123.446 133.289C123.933 139.231 123.137 145.519 121.058 152.152C120.367 154.363 119.019 155.988 117.012 157.028C115.005 158.067 112.899 158.238 110.694 157.542ZM42.2875 41.4583L44.775 49.75H104.475L106.963 41.4583H42.2875ZM44.775 66.3333C41.1819 66.3333 37.9344 65.2637 35.0323 63.1245C32.1302 60.9852 30.1264 58.1854 29.0208 54.725L24.875 41.4583H16.5833C12.0229 41.4583 8.12031 39.8359 4.8755 36.5911C1.63069 33.3463 0.00552778 29.4409 0 24.875V16.5833C0 12.0229 1.62517 8.12031 4.8755 4.8755C8.12583 1.63069 12.0284 0.00552778 16.5833 0H132.667C137.227 0 141.132 1.62517 144.383 4.8755C147.633 8.12583 149.256 12.0284 149.25 16.5833V24.875C149.25 29.4354 147.628 33.3408 144.383 36.5911C141.138 39.8415 137.233 41.4639 132.667 41.4583H124.375L118.985 55.5542C117.742 58.7326 115.738 61.3252 112.974 63.3317C110.21 65.3383 107.101 66.3389 103.646 66.3333H44.775Z"/></symbol>
 <symbol id="i-seat" viewBox="-16.67 -16.67 200 200"><path fill="currentColor" stroke="none" fill-rule="evenodd" clip-rule="evenodd" d="M22.9167 166.667H143.75C149.828 166.667 155.657 164.252 159.955 159.955C164.252 155.657 166.667 149.828 166.667 143.75V100C166.667 89.9333 159.5 81.5167 150 79.5833V39.5833C150 34.3852 148.976 29.2379 146.987 24.4354C144.998 19.633 142.082 15.2693 138.406 11.5937C134.731 7.91804 130.367 5.00235 125.565 3.0131C120.762 1.02385 115.615 0 110.417 0H56.25C45.7519 0 35.6837 4.17037 28.2604 11.5937C20.837 19.017 16.6667 29.0852 16.6667 39.5833V79.5833C7.16667 81.5167 0 89.9333 0 100V143.75C0 149.828 2.41443 155.657 6.71214 159.955C11.0098 164.252 16.8388 166.667 22.9167 166.667ZM56.25 12.5C49.0671 12.5 42.1783 15.3534 37.0992 20.4325C32.0201 25.5116 29.1667 32.4004 29.1667 39.5833V80.9083C32.8816 82.5296 36.0428 85.1994 38.2631 88.5905C40.4834 91.9816 41.6662 95.9467 41.6667 100V113.958C55.2377 110.21 69.2541 108.317 83.3333 108.333C97.75 108.333 111.725 110.292 125 113.958V100C125 91.4667 130.15 84.125 137.5 80.9083V39.5833C137.5 32.4004 134.647 25.5116 129.567 20.4325C124.488 15.3534 117.6 12.5 110.417 12.5H56.25ZM125 126.975C111.489 122.887 97.4489 120.818 83.3333 120.833C68.8333 120.833 54.8417 122.983 41.6667 126.975V154.167H125V126.975ZM20.8333 91.6667C16.25 91.6667 12.5 95.4167 12.5 100V143.75C12.5 149.5 17.1667 154.167 22.9167 154.167H29.1667V100C29.1667 95.4167 25.4167 91.6667 20.8333 91.6667ZM137.5 100C137.5 95.4167 141.25 91.6667 145.833 91.6667C150.417 91.6667 154.167 95.4167 154.167 100V143.75C154.167 149.5 149.5 154.167 143.75 154.167H137.5V100Z"/></symbol>
 <symbol id="i-seat0" viewBox="0 0 74 74"><path fill="currentColor" stroke="none" fill-rule="evenodd" clip-rule="evenodd" d="M14.6462 67.8332H59.3545C61.6033 67.8332 63.76 66.9398 65.3502 65.3497C66.9403 63.7595 67.8337 61.6028 67.8337 59.354V43.1665C67.8337 39.4418 65.182 36.3277 61.667 35.6123V20.8123C61.667 18.889 61.2882 16.9845 60.5521 15.2076C59.8161 13.4307 58.7373 11.8162 57.3773 10.4562C56.0173 9.09618 54.4028 8.01737 52.6259 7.28135C50.849 6.54533 48.9445 6.1665 47.0212 6.1665H26.9795C23.0952 6.1665 19.37 7.70954 16.6233 10.4562C13.8767 13.2028 12.3337 16.928 12.3337 20.8123V35.6123C8.81866 36.3277 6.16699 39.4418 6.16699 43.1665V59.354C6.16699 61.6028 7.06033 63.7595 8.65048 65.3497C10.2406 66.9398 12.3973 67.8332 14.6462 67.8332ZM26.9795 10.7915C24.3218 10.7915 21.773 11.8473 19.8937 13.7265C18.0144 15.6058 16.9587 18.1546 16.9587 20.8123V36.1026C18.3332 36.7025 19.5028 37.6903 20.3243 38.945C21.1458 40.1997 21.5835 41.6668 21.5837 43.1665V48.3311C26.6049 46.9441 31.791 46.244 37.0003 46.2498C42.3345 46.2498 47.5052 46.9744 52.417 48.3311V43.1665C52.417 40.0092 54.3225 37.2928 57.042 36.1026V20.8123C57.042 18.1546 55.9862 15.6058 54.107 13.7265C52.2277 11.8473 49.6789 10.7915 47.0212 10.7915H26.9795ZM52.417 53.1473C47.418 51.6348 42.2231 50.8691 37.0003 50.8748C31.6353 50.8748 26.4584 51.6703 21.5837 53.1473V63.2082H52.417V53.1473ZM13.8753 40.0832C12.1795 40.0832 10.792 41.4707 10.792 43.1665V59.354C10.792 61.4815 12.5187 63.2082 14.6462 63.2082H16.9587V43.1665C16.9587 41.4707 15.5712 40.0832 13.8753 40.0832ZM57.042 43.1665C57.042 41.4707 58.4295 40.0832 60.1253 40.0832C61.8212 40.0832 63.2087 41.4707 63.2087 43.1665V59.354C63.2087 61.4815 61.482 63.2082 59.3545 63.2082H57.042V43.1665Z"/></symbol>
 <symbol id="i-seat1" viewBox="0 0 74 74"><path fill="currentColor" stroke="none" fill-rule="evenodd" clip-rule="evenodd" d="M47.0212 6.1665H26.9795C18.9012 6.1665 12.3337 12.734 12.3337 20.8123V33.9165C18.2845 33.9165 23.1253 38.7573 23.1253 44.7082V47.9303C32.1903 45.7103 41.8103 45.7103 50.8753 47.9303V44.7082C50.8753 38.7573 55.7162 33.9165 61.667 33.9165V20.8123C61.667 12.734 55.0995 6.1665 47.0212 6.1665ZM14.6462 67.8332H18.5003V44.7082C18.5003 43.0727 17.8506 41.5042 16.6942 40.3477C15.5377 39.1912 13.9692 38.5415 12.3337 38.5415C10.6982 38.5415 9.12964 39.1912 7.97317 40.3477C6.81669 41.5042 6.16699 43.0727 6.16699 44.7082V59.354C6.17107 61.6016 7.06571 63.7559 8.65498 65.3452C10.2443 66.9344 12.3986 67.8291 14.6462 67.8332ZM55.5003 67.8332H59.3545C61.6021 67.8291 63.7564 66.9344 65.3457 65.3452C66.9349 63.7559 67.8296 61.6016 67.8337 59.354V44.7082C67.8337 43.0727 67.184 41.5042 66.0275 40.3477C64.871 39.1912 63.3025 38.5415 61.667 38.5415C60.0315 38.5415 58.463 39.1912 57.3065 40.3477C56.15 41.5042 55.5003 43.0727 55.5003 44.7082V67.8332ZM23.1253 67.8332V52.7403C32.1595 50.3198 41.8412 50.3198 50.8753 52.7403V67.8332H23.1253Z"/></symbol>
 <symbol id="i-search" viewBox="0 0 200 200"><path fill="currentColor" stroke="none" d="M150 85C150 99.3438 145.344 112.594 137.5 123.344L177.062 162.938C180.969 166.844 180.969 173.188 177.062 177.094C173.156 181 166.812 181 162.906 177.094L123.344 137.5C112.594 145.344 99.3437 150 85 150C49.0937 150 20 120.906 20 85C20 49.0938 49.0937 20 85 20C120.906 20 150 49.0938 150 85ZM85 130C109.844 130 130 109.844 130 85C130 60.1562 109.844 40 85 40C60.1562 40 40 60.1562 40 85C40 109.844 60.1562 130 85 130Z"/></symbol>
 <symbol id="i-lock" viewBox="0 0 24 24"><rect x="5" y="11" width="14" height="10" rx="2" fill="currentColor" stroke="none"/><path d="M8 11V8a4 4 0 0 1 8 0v3" stroke-width="2.4"/></symbol>
 <symbol id="i-unlock" viewBox="0 0 24 24"><rect x="5" y="11" width="14" height="10" rx="2" fill="currentColor" stroke="none"/><path d="M8 11V8a4 4 0 0 1 7.7-1.5" stroke-width="2.4"/></symbol>
 <symbol id="i-x" viewBox="0 0 24 24"><path d="M5 5l14 14M19 5L5 19"/></symbol>
 <symbol id="i-chev" viewBox="0 0 24 24"><path d="M9 5l7 7-7 7"/></symbol>
 <symbol id="i-down" viewBox="0 0 24 24"><path d="M5 9l7 7 7-7"/></symbol>
</svg>

<aside class="sidebar">
  <div class="menu-title">MENU</div>
  <nav class="nav">
    <a href="#env" data-page="env"><svg class="icon"><use href="#i-env"/></svg><span>Environment</span></a>
    <div class="subnav" id="subnav-env">
      <a href="#env" data-open="light"><svg class="icon"><use href="#i-bulb"/></svg><span>Light</span></a>
      <a href="#env" data-open="temp"><svg class="icon"><use href="#i-temp"/></svg><span>Temperature</span></a>
      <a href="#env" data-open="smoke"><svg class="icon"><use href="#i-smoke"/></svg><span>Smoke</span></a>
    </div>
    <a href="#book" data-page="book"><svg class="icon"><use href="#i-book"/></svg><span>Book</span></a>
    <div class="subnav" id="subnav-book">
      <a href="#book" data-open="seat"><svg class="icon"><use href="#i-seat"/></svg><span>Seat</span></a>
      <a href="#book" data-open="search"><svg class="icon"><use href="#i-search"/></svg><span>Search</span></a>
    </div>
  </nav>
  <button class="sidebar-toggle" id="sidebar-toggle" aria-label="เปิด/ปิดเมนู">
    <svg class="icon"><use href="#i-chev"/></svg>
  </button>
</aside>

<main>
  <section class="page" id="page-home">
    <h1 class="title">Smart Library</h1>
  </section>

  <section class="page" id="page-env">
    <div class="cards">
      <div class="card"><svg class="icon"><use href="#i-bulb"/></svg><h2>Light</h2><button class="manage" data-open="light">Manage</button></div>
      <div class="card"><svg class="icon"><use href="#i-temp"/></svg><h2>Temperature</h2><button class="manage" data-open="temp">Manage</button></div>
      <div class="card"><svg class="icon"><use href="#i-smoke"/></svg><h2>Smoke</h2><button class="manage" data-open="smoke">Manage</button></div>
    </div>
  </section>

  <section class="page" id="page-book">
    <div class="cards tall">
      <div class="card"><svg class="icon"><use href="#i-seat"/></svg><h2>Seat</h2><button class="manage" data-open="seat">Manage</button></div>
      <div class="card"><svg class="icon"><use href="#i-search"/></svg><h2>Search</h2><button class="manage" data-open="search">Manage</button></div>
    </div>
  </section>
</main>

<div class="overlay" id="overlay">

  <div class="modal" id="modal-light">
    <div class="modal-head"><h3>Light</h3><button class="close" data-close aria-label="ปิด"><svg class="icon"><use href="#i-x"/></svg></button></div>
    <div class="modal-body">
      <svg class="icon hero"><use id="light-icon" href="#i-bulb"/></svg>
      <div class="rows">
        <div class="row">
          <span class="pill">Status</span>
          <button class="box" id="light-toggle">Off</button>
          <svg class="icon lock"><use id="light-lock" href="#i-lock"/></svg>
        </div>
        <div class="row">
          <span class="pill">Mode</span>
          <div class="dropdown-wrap">
            <div class="dropdown" id="mode-dropdown">
              <button id="mode-button"><span id="mode-current">Auto</span><svg class="icon"><use href="#i-down"/></svg></button>
              <button class="other" id="mode-other">Manual</button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <div class="modal" id="modal-temp">
    <div class="modal-head"><h3>Temperature</h3><button class="close" data-close aria-label="ปิด"><svg class="icon"><use href="#i-x"/></svg></button></div>
    <div class="modal-body">
      <svg class="icon hero"><use href="#i-temp"/></svg>
      <div class="rows">
        <div class="row wide">
          <span class="pill">Fan</span>
          <div class="segment" id="fan">
            <button data-value="1">On</button>
            <button data-value="0">Off</button>
          </div>
        </div>
        <div class="row tight">
          <span class="pill">Air Conditioner</span>
          <div class="box temp-value"><span id="ac-value">25.0</span>&nbsp;°C</div>
        </div>
      </div>
    </div>
  </div>

  <div class="modal" id="modal-smoke">
    <div class="modal-head"><h3>Smoke</h3><button class="close" data-close aria-label="ปิด"><svg class="icon"><use href="#i-x"/></svg></button></div>
    <div class="modal-body loose">
      <svg class="icon hero"><use id="smoke-icon" href="#i-smoke"/></svg>
      <div class="row tight">
        <span class="pill">Status</span>
        <div class="box" id="smoke-status">Normal</div>
      </div>
    </div>
  </div>

  <div class="modal" id="modal-seat">
    <div class="modal-head"><h3>Seat</h3><button class="close" data-close aria-label="ปิด"><svg class="icon"><use href="#i-x"/></svg></button></div>
    <div class="modal-body compact">
      <svg class="icon hero"><use href="#i-seat"/></svg>
      <div class="seats" id="seats" role="img" aria-label="Seats"></div>
      <p class="seat-summary"><span>Available <b id="seat-free">7</b> / <b id="seat-total">7</b></span><span>Occupied <b id="seat-used">0</b></span></p>
    </div>
  </div>

  <div class="modal" id="modal-search">
    <div class="modal-head"><h3>Search</h3><button class="close" data-close aria-label="ปิด"><svg class="icon"><use href="#i-x"/></svg></button></div>
    <div class="modal-body">
      <div class="search">
        <div class="search-bar">
          <button id="search-go" aria-label="ค้นหา"><svg class="icon"><use href="#i-search"/></svg></button>
          <input id="search-input" placeholder="Search" autocomplete="off">
        </div>
      </div>
      <p class="found" id="found">Loading...</p>
      <div class="results" id="results"></div>
    </div>
  </div>

</div>

<div class="alarm" id="smoke-alarm" role="alertdialog" aria-modal="true" aria-labelledby="alarm-title">
  <div class="alarm-card">
    <svg class="icon"><use id="alarm-icon" href="#i-smoke2"/></svg>
    <h3 id="alarm-title">Smoke Detected</h3>
    <div class="row tight"><span class="pill">Status</span><div class="box alarm-status" id="alarm-status">Danger</div></div>
    <div class="alarm-time">
      <span id="alarm-label">Last detected</span>
      <b id="alarm-last">--:--:--</b>
      <small id="alarm-since"></small>
    </div>
    <button class="alarm-sound" id="alarm-sound" hidden>Tap to enable sound</button>
    <button class="manage alarm-ok" id="alarm-ok">OK</button>
  </div>
</div>

<script>
const $ = id => document.getElementById(id);
const $$ = selector => document.querySelectorAll(selector);
const body = document.body;

const storage = {
  get: key => { try { return localStorage[key] } catch {} },
  set: (key, value) => { try { localStorage[key] = value } catch {} },
};

/* Sidebar (มือถือ: เริ่มแบบปิด และปิดเมื่อแตะนอกเมนู) */
const phone = matchMedia('(max-width: 600px)');
function setMenuOpen(open) {
  body.classList.toggle('menu-open', open);
  storage.set('menuOpen', open ? 1 : 0);
}
setMenuOpen(!phone.matches && storage.get('menuOpen') == 1);
$('sidebar-toggle').onclick = () => setMenuOpen(!body.classList.contains('menu-open'));

/* Router */
const PAGES = ['home', 'env', 'book'];
function route() {
  const page = location.hash.slice(1);
  body.dataset.page = PAGES.includes(page) ? page : 'home';
  $$('.nav a[data-page]').forEach(a => a.classList.toggle('active', a.dataset.page === body.dataset.page));
}
addEventListener('hashchange', route);
route();

/* State & API (smoke: 0 = Normal, 1 = Danger / ac: °C ทศนิยม 1 ตำแหน่ง) */
let state = { lm: 'auto', lo: 0, fan: 0, ac: 25, smoke: 0, seat: 0, seats: 7, fol: -1 };   // seat = จำนวนโต๊ะที่มีคน
let pending = 0;

async function getJSON(url) {
  try { const res = await fetch(url); if (res.ok) return await res.json() } catch {}
}

async function refresh() {
  if (pending) return;
  const data = await getJSON('/api/state');
  if (data && !pending) { state = data; render() }
}

async function update(key, value) {
  state[key] = value;
  render();
  pending++;
  const data = await getJSON(`/api/set?${key}=${value}`);
  pending--;
  if (data) { state = data; render() }
}

/* Render */
function render() {
  const manual = state.lm === 'manual';
  $('light-icon').setAttribute('href', state.lo ? '#i-bulbon' : '#i-bulb');
  $('light-toggle').textContent = state.lo ? 'On' : 'Off';
  $('light-toggle').disabled = !manual;
  $('light-lock').setAttribute('href', manual ? '#i-unlock' : '#i-lock');
  $('mode-current').textContent = manual ? 'Manual' : 'Auto';
  $('mode-other').textContent = manual ? 'Auto' : 'Manual';

  $$('#fan button').forEach(b => b.classList.toggle('active', +b.dataset.value === +state.fan));
  $('ac-value').textContent = Number(state.ac).toFixed(1);

  const danger = !!state.smoke;
  $('smoke-status').textContent = danger ? 'Danger' : 'Normal';
  $('smoke-status').classList.toggle('danger', danger);
  $('smoke-icon').setAttribute('href', danger ? '#i-smoke2' : '#i-smoke');
  renderAlarm(danger);

  const seatTotal = +state.seats || 7;
  const seatUsed = Math.min(+state.seat || 0, seatTotal);
  $('seat-total').textContent = seatTotal;
  $('seat-used').textContent = seatUsed;
  $('seat-free').textContent = seatTotal - seatUsed;
  // แถบที่นั่ง 1..N เติมคนจากซ้ายไปขวา (ช่องที่ 1..seatUsed = มีคน)
  const bar = $('seats');
  if (bar.children.length !== seatTotal) {
    bar.innerHTML = Array.from({ length: seatTotal }, (_, i) =>
      `<div class="seat"><b>${i + 1}</b><svg class="icon"><use href="#i-seat0"/></svg></div>`).join('');
  }
  [...bar.children].forEach((seat, i) => {
    const taken = i < seatUsed;
    seat.classList.toggle('taken', taken);
    seat.querySelector('use').setAttribute('href', taken ? '#i-seat1' : '#i-seat0');
  });
  bar.setAttribute('aria-label', `${seatTotal - seatUsed} of ${seatTotal} seats available`);

  $$('.book').forEach(book => book.classList.toggle('followed', +book.dataset.id === state.fol));
}

/* Smoke alert: ขอสถานะทุก 1.5 วินาทีตลอดเวลา เตือนได้ทุกหน้า
   อันตราย = บังทั้งจอ ไม่มีปุ่มปิด / ควันหาย = ค้างไว้บอกเวลา danger ล่าสุด จนกว่าจะกด OK
   เวลาใช้นาฬิกาของเครื่องที่เปิดเว็บ (ESP ไม่มีนาฬิกา) */
let alarm = null;   // { since, last, cleared }
const timeText = d => d.toLocaleString('en-GB', { day: '2-digit', month: 'short', hour: '2-digit', minute: '2-digit', second: '2-digit' });

function renderAlarm(danger) {
  const now = new Date();
  if (danger) {
    if (!alarm || alarm.cleared) alarm = { since: now };
    alarm.last = now;
  } else if (alarm && !alarm.cleared) {
    alarm.cleared = now;
  }
  const box = $('smoke-alarm');
  box.classList.toggle('show', !!alarm);
  setSiren(!!alarm && !alarm.cleared);
  if (!alarm) return;
  const cleared = !!alarm.cleared;
  $('alarm-sound').hidden = cleared || soundOn();
  box.classList.toggle('danger', !cleared);
  box.classList.toggle('cleared', cleared);
  $('alarm-title').textContent = cleared ? 'Smoke Cleared' : 'Smoke Detected';
  $('alarm-status').textContent = cleared ? 'Normal' : 'Danger';
  $('alarm-icon').setAttribute('href', cleared ? '#i-smoke' : '#i-smoke2');
  $('alarm-label').textContent = 'Last danger';           // เวลาล่าสุดที่บอร์ดรายงานว่าอันตราย
  $('alarm-last').textContent = timeText(alarm.last);
  $('alarm-since').textContent = cleared
    ? `Since ${timeText(alarm.since)} · Cleared ${timeText(alarm.cleared)}`
    : `Since ${timeText(alarm.since)}`;
}
$('alarm-ok').onclick = () => { alarm = null; render() };

/* เสียงไซเรน (Yelp) และสั่น: browser เล่นเสียงได้หลังผู้ใช้แตะหน้าเว็บอย่างน้อย 1 ครั้ง */
let audio = null, siren = null;
function unlockAudio() {
  try {
    audio = audio || new (window.AudioContext || window.webkitAudioContext)();
    if (audio.state !== 'running') audio.resume().then(render, () => {});
  } catch {}
}
['pointerdown', 'click', 'keydown'].forEach(type => addEventListener(type, unlockAudio, true));
const soundOn = () => !!audio && audio.state === 'running';

function beep() {
  const tapped = navigator.userActivation ? navigator.userActivation.hasBeenActive : soundOn();
  try { if (tapped && navigator.vibrate) navigator.vibrate([400, 150, 400]) } catch {}
  // Chrome อาจพักระบบเสียงไว้ (เช่นหลังจอดับ) ถ้าเคยแตะหน้าเว็บแล้วปลุกกลับได้เลย
  if (audio && audio.state !== 'running' && tapped) audio.resume().catch(() => {});
  if (!soundOn()) return;
  const t = audio.currentTime + 0.02;
  // Yelp Siren: ไต่ความถี่ 700 -> 1500 Hz ถี่ 4 รอบใน 1 วินาที (setSiren เรียกทุก 1 วินาที)
  for (let i = 0; i < 4; i++) {
    const t0 = t + i * 0.25, dur = 0.25;
    const osc = audio.createOscillator(), gain = audio.createGain();
    osc.type = 'sawtooth';
    osc.frequency.setValueAtTime(700, t0);
    osc.frequency.linearRampToValueAtTime(1500, t0 + dur);
    gain.gain.setValueAtTime(0.0001, t0);
    gain.gain.exponentialRampToValueAtTime(0.45, t0 + 0.015);
    gain.gain.setValueAtTime(0.45, t0 + dur - 0.03);
    gain.gain.exponentialRampToValueAtTime(0.0001, t0 + dur);
    osc.connect(gain).connect(audio.destination);
    osc.start(t0);
    osc.stop(t0 + dur + 0.02);
  }
}

function setSiren(on) {
  if (on && !siren) { beep(); siren = setInterval(beep, 1000) }
  else if (!on && siren) { clearInterval(siren); siren = null }
}
$('alarm-sound').onclick = () => { unlockAudio(); beep(); render() };
// ทดสอบเสียง: แตะกล่องสถานะในหน้าต่าง Smoke
$('smoke-status').onclick = () => { unlockAudio(); setTimeout(beep, 50) };
setInterval(refresh, 1500);
refresh();

/* Modal */
function openModal(name) {
  $$('.modal').forEach(m => m.classList.toggle('show', m.id === 'modal-' + name));
  $('overlay').classList.add('show');
  render();
  refresh();
  if (name === 'search') { loadBooks(); setTimeout(() => $('search-input').focus(), 50) }
}

function closeModal() {
  $('overlay').classList.remove('show');
  $('mode-dropdown').classList.remove('open');
}

document.addEventListener('click', e => {
  const target = e.target;
  const opener = target.closest('[data-open]');
  if (phone.matches && (opener || !target.closest('.sidebar'))) setMenuOpen(false);
  if (opener) openModal(opener.dataset.open);
  if (target.closest('[data-close]') || target.id === 'overlay') closeModal();
  if (!target.closest('#mode-dropdown')) $('mode-dropdown').classList.remove('open');
});
addEventListener('keydown', e => { if (e.key === 'Escape') closeModal() });

/* Light */
$('light-toggle').onclick = () => update('lo', state.lo ? 0 : 1);
$('mode-button').onclick = () => $('mode-dropdown').classList.toggle('open');
$('mode-other').onclick = () => {
  $('mode-dropdown').classList.remove('open');
  update('lm', state.lm === 'auto' ? 'manual' : 'auto');
};

/* Fan */
$$('#fan button').forEach(b => b.onclick = () => update('fan', +b.dataset.value));

/* Search: [id, name, zone, shelf, count] / zone, shelf: 1 = แดง, 2 = น้ำเงิน, 3 = เขียว */
const DEMO_BOOKS = [
  [1, 'Book No.1', 1, 1, 3], [2, 'Book No.2', 1, 2, 1], [3, 'Book No.3', 1, 3, 2],
  [4, 'Book No.4', 2, 1, 0], [5, 'Book No.5', 2, 2, 2], [6, 'Book No.6', 2, 3, 1],
  [7, 'Book No.7', 3, 1, 4], [8, 'Book No.8', 3, 2, 2], [9, 'Book No.9', 3, 3, 1],
];
const COLORS = { 1: ['Red', '#E57373'], 2: ['Blue', '#64B5F6'], 3: ['Green', '#81C784'] };
let books = [];

async function loadBooks() {
  if (!books.length) books = await getJSON('/api/books') || DEMO_BOOKS;
  showResults();
}

const escapeHTML = s => String(s).replace(/[&<>"]/g, c => '&#' + c.charCodeAt(0) + ';');

function findBooks(query) {
  query = query.trim().toLowerCase();
  return query ? books.filter(([, name]) => name.toLowerCase().includes(query)) : books;
}

function bookCard([id, name, zone, shelf, count]) {
  const [zoneName, zoneHex] = COLORS[zone];
  const [shelfName, shelfHex] = COLORS[shelf];
  return `
    <div class="book" data-id="${id}">
      <h4>${escapeHTML(name)}</h4>
      <p>Available : ${count}</p>
      <button class="follow">Follow</button>
      <div class="location"><span>Zone</span><b>${zoneName}</b><i style="--color:${zoneHex}"></i></div>
      <div class="location"><span>Shelf</span><b>${shelfName}</b><i style="--color:${shelfHex}"></i></div>
    </div>`;
}

/* แสดงทุกเล่มเมื่อช่องค้นหาว่าง พิมพ์แล้วกรองทันที */
function showResults() {
  const query = $('search-input').value.trim();
  const list = findBooks(query);
  $('found').textContent = query
    ? `Found : ${list.length} / ${books.length} title(s)`
    : `All books : ${books.length} title(s)`;
  $('results').innerHTML = list.length
    ? list.map(bookCard).join('')
    : '<p class="no-result">No results</p>';
  render();
}

$('search-input').oninput = showResults;
$('search-input').onkeydown = e => { if (e.key === 'Enter') showResults() };
$('search-go').onclick = showResults;

$('results').onclick = e => {
  const button = e.target.closest('.follow');
  if (!button) return;
  const id = +button.closest('.book').dataset.id;
  update('fol', state.fol === id ? -1 : id);
};
</script>
</body>
</html>
)rawliteral";