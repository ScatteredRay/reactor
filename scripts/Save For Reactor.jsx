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

    createAlphaFromTransparency();

    var saturate = saturateTransparency(doc, layer);

    var file = new File(path);

    var opts = new BMPSaveOptions();
    opts.alphaChannels = true;
    opts.depth = BMPDepthType.BMP_A8R8G8B8;
    opts.flipRowOrder = false;
    opts.osType = OperatingSystem.WINDOWS
    opts.rleCompression = false;

    doc.saveAs(file, opts, true, Extension.LOWERCASE);

    cleanupSaturate(saturate);

    clearAlpha(doc);
}

function createAlphaFromTransparency() {
    var idMk = charIDToTypeID( "Mk  " );
        var desc3 = new ActionDescriptor();
        var idNw = charIDToTypeID( "Nw  " );
        var idChnl = charIDToTypeID( "Chnl" );
        desc3.putClass( idNw, idChnl );
        var idUsng = charIDToTypeID( "Usng" );
            var desc4 = new ActionDescriptor();
            var idT = charIDToTypeID( "T   " );
                var ref2 = new ActionReference();
                var idChnl = charIDToTypeID( "Chnl" );
                var idChnl = charIDToTypeID( "Chnl" );
                var idTrsp = charIDToTypeID( "Trsp" );
                ref2.putEnumerated( idChnl, idChnl, idTrsp );
                var idLyr = charIDToTypeID( "Lyr " );
                var idOrdn = charIDToTypeID( "Ordn" );
                var idMrgd = charIDToTypeID( "Mrgd" );
                ref2.putEnumerated( idLyr, idOrdn, idMrgd );
            desc4.putReference( idT, ref2 );
            var idSrctwo = charIDToTypeID( "Src2" );
                var ref3 = new ActionReference();
                var idChnl = charIDToTypeID( "Chnl" );
                var idChnl = charIDToTypeID( "Chnl" );
                var idTrsp = charIDToTypeID( "Trsp" );
                ref3.putEnumerated( idChnl, idChnl, idTrsp );
                var idLyr = charIDToTypeID( "Lyr " );
                var idOrdn = charIDToTypeID( "Ordn" );
                var idMrgd = charIDToTypeID( "Mrgd" );
                ref3.putEnumerated( idLyr, idOrdn, idMrgd );
            desc4.putReference( idSrctwo, ref3 );
        var idClcl = charIDToTypeID( "Clcl" );
        desc3.putObject( idUsng, idClcl, desc4 );
    executeAction( idMk, desc3, DialogModes.NO );
}

function clearAlpha(doc) {
    for(var i=0; i<doc.channels.length; i++) {
        if(doc.channels[i].name == "Alpha 1") {
            doc.channels[i].remove()
            i--;
        }
    }
}

function saturateTransparency(doc, layer) {
    var duplicateTimes = 256; // In theory 255 times should be perfect, we could perhaps get by with less.

    var res = [];

    for(var i=0; i< duplicateTimes; i++) {
        layer = layer.duplicate(doc, ElementPlacement.INSIDE);
        if(layer.typename == "LayerSet") {
            layer = layer.merge();
        }
        res.push(layer);
    }
    return res;
}

function cleanupSaturate(res) {

    while(res.length > 0) {
        var l = res.pop();
        l.remove();
    }
}

function main() {
    if(app.documents.length <= 0) {
        display("Must have document open.");
        return 'cancel'
    }

    var orig = app.activeDocument;
    var doc = orig.duplicate();
    // I don't think this is needed.
    app.activeDocument = doc;

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

    app.activeDocument = orig;
    doc.close(SaveOptions.DONOTSAVECHANGES);
}

main();
