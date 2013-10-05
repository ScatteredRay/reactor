var typedata = {
  "Environment": {
    "type" : "object",
    "contents": {
      "Scattering" : {
        "type": "object",
        "contents": {
          "LightLocation": {
            "type": "object",
            "contents": {
              "X": {
                "type": "float",
                "value": 0.3,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              },
              "Y": {
                "type": "float",
                "value": 0.6,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              },
              "Z": {
                "type": "float",
                "value": 0.0,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              }
            }
          },
          "LightColor": {
            "type": "object",
            "contents": {
              "X": {
                "type": "float",
                "value": 1.0,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              },
              "Y": {
                "type": "float",
                "value": 0.4,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              },
              "Z": {
                "type": "float",
                "value": 0.0,
                "range": {
                  "min": 0.0,
                  "max": 1.0
                }
              }
            }
          },
          "LightPower": {
            "type": "float",
            "value": 1.0,
            "range": {
              "min": 0.0,
              "max": 10.0
            }
          },
          "NumSamples": {
            "type": "float",
            "value": 50.0,
            "range": {
              "min": 0.0,
              "max": 100.0
            }
          },
          "Weight": {
            "type": "float",
            "value": 0.1,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "Decay": {
            "type": "float",
            "value": 0.9,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "Extinction": {
            "type": "float",
            "value": 0.01,
            "range": {
              "min": 0.0,
              "max": 0.10
            }
          },
          "Ambient": {
            "type": "float",
            "value": 0.2,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "Angular": {
            "type": "float",
            "value": 0.2,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "Rayleigh": {
            "type": "float",
            "value": 0.2,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "Mie": {
            "type": "float",
            "value": 0.55,
            "range": {
              "min": 0.0,
              "max": 1.0
            }
          },
          "MieEccentricity": {
            "type": "float",
            "value": 0.5,
            "range": {
              "min": 0.0,
              "max": 0.99
            }
          },
        }
      }
    }
  }
};

function getFoo() {
  // Just keepping this here.
  var req = new XMLHttpRequest();

  req.open("PUT", "http://127.0.0.1:25115/object" + path, true);
  req.onreadystatechange = function() {
    if(req.readyState == 4 && req.status == 200) {
      alert(req.responseText);
    }
  };
  req.send();
}

function sendPropertyUpdate(path, value) {
  var req = new XMLHttpRequest();
  req.open("PUT", "http://127.0.0.1:25115/object" + path, true);
  // value
  req.send(value.toString());
}

function propertyUpdateValue(property, val) {
  property.value = val;
  sendPropertyUpdate(property.path, val);
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

  function updateFromInput() {
    var f = parseFloat(el.value);
    updateValue(f);
  }

  el.onblur = updateFromInput;

  el.onkeypress = function(e) {
    var key = e.which;
    if(key == 13) {
      updateFromInput();
    }
  };

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

function setupProperties(properties, parent, path) {
  for(var p in properties) {
    var prop = properties[p];
    prop.path = path + '/' + p;
    var e = addProperty(p, prop, parent);
    if(prop.contents) {
      setupProperties(prop.contents, e, prop.path);
    }
  }
}

function onInit()
{
  var PropertiesElem = document.getElementById('properties')

  setupProperties(typedata, PropertiesElem, "");
}
