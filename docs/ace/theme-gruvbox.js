ace.define("ace/theme/gruvbox",[], function(require, exports, module) {

exports.isDark = true;
exports.cssClass = "ace-gruvbox";
exports.cssText = ".ace-gruvbox .ace_gutter-active-line {\
background-color: #3C3836;\
}\
.ace-gruvbox {\
color: #EBDAB4;\
background-color: #1D2021;\
}\
.ace-gruvbox .ace_invisible {\
color: #504945;\
}\
.ace-gruvbox .ace_marker-layer .ace_selection {\
background: rgba(179, 101, 57, 0.75)\
}\
.ace-gruvbox.ace_multiselect .ace_selection.ace_start {\
box-shadow: 0 0 3px 0px #002240;\
}\
.ace-gruvbox .ace_keyword {\
color: var(--orange);\
}\
.ace-gruvbox .ace_comment {\
font-style: italic;\
color: #928375;\
}\
.ace-gruvbox .ace-statement {\
color: red;\
}\
.ace-gruvbox .ace_variable {\
color: var(--blue);\
}\
.ace-gruvbox .ace_constant {\
color: var(--yellow);\
}\
.ace-gruvbox .ace_string {\
color: var(--aqua);\
}\
.ace-gruvbox .ace_marker-layer .ace_active-line {\
background: #3C3836;\
}\
.ace-gruvbox .ace_marker-layer .ace_selected-word {\
border-radius: 4px;\
border: 8px solid #3f475d;\
}";

var dom = require("../lib/dom");
dom.importCssString(exports.cssText, exports.cssClass);

});
                (function() {
                    ace.require(["ace/theme/gruvbox"], function(m) {
                        if (typeof module == "object" && typeof exports == "object" && module) {
                            module.exports = m;
                        }
                    });
                })();
            