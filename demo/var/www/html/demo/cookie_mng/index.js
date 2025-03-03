
function setCookie() {
    const name = document.getElementById("cookie-name").value;
    const value = document.getElementById("cookie-value").value;
    fetch(`/cgi-bin/cookie_manager.py?action=set&name=${encodeURIComponent(name)}&value=${encodeURIComponent(value)}`)
        .then(response => response.text())
        .then(data => alert(data));
}

function getCookies() {
    fetch("/cgi-bin/cookie_manager.py?action=get")
        .then(response => response.text())
        .then(data => document.getElementById("cookie-display").innerText = data);
}

function deleteCookie() {
    const name = document.getElementById("cookie-name").value;
    fetch(`/cgi-bin/cookie_manager.py?action=delete&name=${encodeURIComponent(name)}`)
        .then(response => response.text())
        .then(data => alert(data));
}
