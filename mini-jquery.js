// 
// mini-jquery.js
//
// Matthew Kosarek
// 
// 2021-08-08
//
// JQuery is huge and I use the smallest portion of it.
// This is my attempt to make a JQuery lookalike that does
// enough for me, but doesn't bloat things.
// 

function MiniJQueryObject(arg) {
    this.elementList = [];
    if (typeof arg === 'string') {
        arg = arg.trim();

        if (arg[0] === '<' && arg[arg.length - 1] === '>') {
            // Trying to create an html element
            arg = arg.substring(1, arg.length - 1);
            var element = document.createElement(arg);
            this.elementList.push(element);
        } else {
            // Trying to query an html element
            
            switch (arg[0]) {
                case '#': {
                    arg = arg.substring(1);
                    var idElement = document.getElementById(arg);
                    if (idElement) {
                        this.elementList.push(idElement);
                    }
                    break;
                }
                case '.': {
                    arg = arg.substring(1);
                    this.fromHTMLCollection(document.getElementsByClassName(arg));
                    break;
                }
                default: {
                    this.fromHTMLCollection(document.getElementsByTagName(arg));
                    break;
                }
            }
        }
    } else if (arg instanceof HTMLElement) {
        this.elementList.push(arg);
    } else {
        console.error('Unknown object: ', arg);
    }
}

MiniJQueryObject.prototype.fromHTMLCollection = function(collection) {
    for (var i = 0; i < collection.length; i++) {
        this.elementList.push(collection.item(i));
    }
};

MiniJQueryObject.prototype.append = function(otherMinJqueryObject) {
    this.elementList.forEach(function(element) {
        otherMinJqueryObject.elementList.forEach(function(otherElement) {
            element.appendChild(otherElement);
        });
    });
    return this;
};

MiniJQueryObject.prototype.appendTo = function(otherMinJqueryObject) {
    otherMinJqueryObject.append(this);
    return this;
};

MiniJQueryObject.prototype.empty = function() {
    this.elementList.forEach(function(element) {
        element.innerHTML = '';
    });
    return this;
};

MiniJQueryObject.prototype._set = function(field, value) {
    this.elementList.forEach(function(element) {
        element[field] = value;
    });
    return this;
};

MiniJQueryObject.prototype.type = function(type) {
    return this._set('type', type);
};

MiniJQueryObject.prototype.val = function(val) {
    return this._set('value', val);
};

MiniJQueryObject.prototype.disabled = function(val) {
    return this._set('disabled', val ? 'true' : undefined);
};

MiniJQueryObject.prototype.on = function(event, callback) {
    this.elementList.forEach(function(element) {
        element.addEventListener(event, callback);
    });
    
    return this;
};

MiniJQueryObject.prototype.addClass = function(className) {
    this.elementList.forEach(function(element) {
        element.classList.add(className);
    });
    return this;
};

MiniJQueryObject.prototype.removeClass = function(className) {
    this.elementList.forEach(function(element) {
        element.classList.remove(className);
    });
    return this;
};

var $ = function(arg) {
    return new MiniJQueryObject(arg);
};
window.$ = $;
