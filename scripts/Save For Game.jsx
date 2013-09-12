#target photoshop

$.localize = false;

function display(str) {
    if(app.playbackDisplaydialogs != DialogModes.NO) {
        alert(str);
    }
}

function saveLayerAs(doc, layer, path) {
    var layers = doc.layers;
    for(var i=0; i<layers.length; i++) {
        if(layer === layers[i]) {
            layers[i].visible = true;
        }
        else {
            layers[i].visible = false;
        }
    }

    alert("Saving as " + path);

    var file = new File(path);

    var opts = new BMPSaveOptions();
    opts.alphaChannels = true;
    opts.depth = BMPDepthType.BMP_A8R8G8B8;
    opts.flipRowOrder = false;
    opts.osType = OperatingSystem.WINDOWS
    opts.rleCompression = false;

    doc.saveAs(file, opts, true, Extension.LOWERCASE);
}

function main() {
    if(app.documents.length <= 0) {
        display("Must have document open.");
        return 'cancel'
    }

    var orig = app.activeDocument;
    var doc = orig.duplicate();

    var savePath;

    try {
        savePath = orig.path + '/';
    }
    catch(e) {
        display("File must have a path, save the file first.");
        return 'cancel'
    }

    var layers = doc.layers;
    for(var i=0; i<layers.length; i++) {
        var layerN = parseInt(layers[i].name);
        if(!isNaN(layerN)) {
            // Numeric floating layer.
            saveLayerAs(doc, layers[i], savePath + layerN + ".bmp"); 
        }
    }

    doc.close(SaveOptions.DONOTSAVECHANGES);
}

main();
