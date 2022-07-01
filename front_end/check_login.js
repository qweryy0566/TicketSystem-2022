function cleanCookie() {
  document.cookie = "";
}
function logout() {
  var cur_user = document.getElementById("cur_user").innerHTML;
  var xml = new XMLHttpRequest();
  xml.open("GET", "logout.php?username=" + cur_user, true);
  xml.send();
  xml.onreadystatechange = function () {
    if (xml.readyState == 4 && xml.status == 200) {
      cleanCookie();
      if (xml.responseText == "0") alert("注销成功。");
      else alert("用户已登出！");
      location.reload();
    }
  }
}
function checkLogin(username) {
  var xml = new XMLHttpRequest();
  xml.open("GET", "is_login.php?username=" + username, true);
  xml.send();
  xml.onreadystatechange = function () {
    if (xml.readyState == 4 && xml.status == 200) {
      if (xml.responseText == "0") {
        var menu = document.getElementById("right_menu");
        menu.innerHTML = `<a href="current_user.html" class="item" id="cur_user">${username}</a>`;
        menu.innerHTML += "<a class=\"item\" onclick=\"logout()\">注销</a>";
      }
    }
  }
}
function getUser() {
  var ca = document.cookie.split(';');
  for (var i = 0; i < ca.length; i++) {
    var c = ca[i].trim();
    if (c.indexOf("UserName=") == 0)
      return c.substring(9, c.length);
  }
  return "";
}
window.addEventListener("load", function () {
  var username = getUser();
  checkLogin(username);
})