var req = new XMLHttpRequest();

req.open("GET", "http://127.0.0.1:25115/object/test", true);
req.onreadystatechange = function() {
  if(req.readyState == 4 && req.status == 200) {
    alert(req.responseText);
  }
};
req.send();