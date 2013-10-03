var typedata = {
  "Environment": {
    "type" : "object",
    "contents": {
      "Scattering" : {
        "type": "object",
        "contents": {
          "LightPower": {
            "type": "float",
            "value": 1.0,
            "range": {
              "min": 0.0,
              "max": 10.0
            }
          }
        }
      }
    }
  }
};

var req = new XMLHttpRequest();

req.open("GET", "http://127.0.0.1:25115/object/test", true);
req.onreadystatechange = function() {
  if(req.readyState == 4 && req.status == 200) {
    alert(req.responseText);
  }
};
req.send();

function propertyUpdateValue(property, val) {
  property.value = val;
}

function elem(type, parent, classes) {
  var e = document.createElement(type);
  if(typeof(classes) === 'string') {
    e.classList.add(classes);
  }
  else if (classes instanceof Array) {
    for(var i=0; i<classes.length; i++) {
      e.classList.add(classes[i]);
    }
  }
  parent.appendChild(e);
  return e;
}

function floatEdit(property, parent) {
  var el = elem('input', parent, 'floatEdit');
  var last = {'x': 0, 'y': 0};

  function updateValue(val) {
    propertyUpdateValue(property, val);
    el.value = val;

  }

  // Todo: implement mouse capture.
  el.value = property.value;
  el.onmousedown = function(e) {
    last.x = e.clientX;
    last.y = e.clientY
    e.target.onmousemove = function(e) {
      var deltaX = e.clientX - last.x;
      last.x = e.clientX;
      last.y = e.clientY
      var step = 0.1;
      var range = property.range;
      if(range) {
        step = (property.range.max - property.range.min)/100.0;
      }
      var v = property.value + deltaX * step;
      v = Math.max(Math.min(v, range.max), range.min);
      updateValue(v);
    };
    e.target.onmouseup = function(e) {
      e.target.onmousemove = null;
      e.target.onmouseup = null;
    }
  };
}

function createEditFor(property, parent) {
  var editProperty = elem('div', parent, 'EditContainer');
  switch(property.type) {
  case 'object':
    editProperty.classList.add('ObjectEditContainer');
    break;
  case 'float':
    editProperty.classList.add('FloatEditContainer');
    floatEdit(property, editProperty);
    break;
  default:
    break;
  }
  return editProperty;
}

// This returns the children container element.
function addProperty(name, property, parent) {
  var e = elem('div', parent, 'PropContainer');;
  var label = elem('div', e, 'PropLabel');
  label.textContent = name;
  var propEdit = createEditFor(property, e);
  return propEdit;
}

function setupProperties(properties, parent) {
  for(var p in properties) {
    var prop = properties[p];
    var e = addProperty(p, prop, parent);
    if(prop.contents) {
      setupProperties(prop.contents, e);
    }
  }
}

function onInit()
{
  var PropertiesElem = document.getElementById('properties')

  setupProperties(typedata, PropertiesElem);
}
