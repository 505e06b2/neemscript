ace.define("ace/mode/batchfile_highlight_rules",[], function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;

var BatchFileHighlightRules = function() {
    this.$rules = { start: 
       [ { token: 'keyword',
           regex: '(^\\s*)(echo|print|import|sleep|start|pwd|goto|exit|call|setlocal|endlocal|cd|rm|rmdir|ls|pause|output|input|libload|librun|libunload)\\b',
           caseInsensitive: true },
         { token: 'string',
           regex: '(^\\s*)(if|else|fi|switch|case|rof)\\b',
           caseInsensitive: true },
         { token: ['markup', 'string', 'constant', 'constant'],
           regex: '(^\\s*)(for\\s+)(\\w+\\s)(\\w+)', //for var var
           caseInsensitive: true },
		 { token: ['markup', 'keyword', 'constant', 'variable'],
           regex: '(^\\s*)(set|setsystem|setglobal|prompt)(\\s\\w+)(=)', //set var=val
           caseInsensitive: true },
		 { token: ['markup', 'keyword', 'constant'],
           regex: '(^\\s*)(inc|readline|readall)(\\s\\w+)', //inc var
           caseInsensitive: true },
         { token: ['doc.comment', 'comment'],
           regex: '(?:^|\\b)(rem)($|\\s.*$)',
           caseInsensitive: true },
         { token: 'comment.line.colons.dosbatch',
           regex: '(?:^|\\b)::.*$' },
		 { token: 'variable',
			regex: '(==|=|!=|<=|<|>=|>|!)(?=\\w+$)'},
		 { token: 'string',
			regex: '(^\\s*)(:\\w+)'},
		 { include: 'variable' }
	   ],
        variable: [
         { token: 'variable', regex: '!\\w+'},
		 { token: 'constant', regex: '%\\S+%'},
		 { token: 'variable', regex: '!\\S+!'},
		 { token: 'variable', regex: '!'},
		 { token: 'constant', regex: '%'}
		]};
    
    this.normalizeRules();
};

BatchFileHighlightRules.metaData = { name: 'Batch File',
      scopeName: 'source.dosbatch',
      fileTypes: [ 'bat' ] };


oop.inherits(BatchFileHighlightRules, TextHighlightRules);

exports.BatchFileHighlightRules = BatchFileHighlightRules;
});

ace.define("ace/mode/folding/cstyle",[], function(require, exports, module) {
"use strict";

var oop = require("../../lib/oop");
var Range = require("../../range").Range;
var BaseFoldMode = require("./fold_mode").FoldMode;

var FoldMode = exports.FoldMode = function(commentRegex) {
    if (commentRegex) {
        this.foldingStartMarker = new RegExp(
            this.foldingStartMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.start)
        );
        this.foldingStopMarker = new RegExp(
            this.foldingStopMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.end)
        );
    }
};
oop.inherits(FoldMode, BaseFoldMode);

(function() {
    
    this.foldingStartMarker = /([\{\[\(])[^\}\]\)]*$|^\s*(\/\*)/;
    this.foldingStopMarker = /^[^\[\{\(]*([\}\]\)])|^[\s\*]*(\*\/)/;
    this.singleLineBlockCommentRe= /^\s*(\/\*).*\*\/\s*$/;
    this.tripleStarBlockCommentRe = /^\s*(\/\*\*\*).*\*\/\s*$/;
    this.startRegionRe = /^\s*(\/\*|\/\/)#?region\b/;
    this._getFoldWidgetBase = this.getFoldWidget;
    this.getFoldWidget = function(session, foldStyle, row) {
        var line = session.getLine(row);
    
        if (this.singleLineBlockCommentRe.test(line)) {
            if (!this.startRegionRe.test(line) && !this.tripleStarBlockCommentRe.test(line))
                return "";
        }
    
        var fw = this._getFoldWidgetBase(session, foldStyle, row);
    
        if (!fw && this.startRegionRe.test(line))
            return "start"; // lineCommentRegionStart
    
        return fw;
    };

    this.getFoldWidgetRange = function(session, foldStyle, row, forceMultiline) {
        var line = session.getLine(row);
        
        if (this.startRegionRe.test(line))
            return this.getCommentRegionBlock(session, line, row);
        
        var match = line.match(this.foldingStartMarker);
        if (match) {
            var i = match.index;

            if (match[1])
                return this.openingBracketBlock(session, match[1], row, i);
                
            var range = session.getCommentFoldRange(row, i + match[0].length, 1);
            
            if (range && !range.isMultiLine()) {
                if (forceMultiline) {
                    range = this.getSectionRange(session, row);
                } else if (foldStyle != "all")
                    range = null;
            }
            
            return range;
        }

        if (foldStyle === "markbegin")
            return;

        var match = line.match(this.foldingStopMarker);
        if (match) {
            var i = match.index + match[0].length;

            if (match[1])
                return this.closingBracketBlock(session, match[1], row, i);

            return session.getCommentFoldRange(row, i, -1);
        }
    };
    
    this.getSectionRange = function(session, row) {
        var line = session.getLine(row);
        var startIndent = line.search(/\S/);
        var startRow = row;
        var startColumn = line.length;
        row = row + 1;
        var endRow = row;
        var maxRow = session.getLength();
        while (++row < maxRow) {
            line = session.getLine(row);
            var indent = line.search(/\S/);
            if (indent === -1)
                continue;
            if  (startIndent > indent)
                break;
            var subRange = this.getFoldWidgetRange(session, "all", row);
            
            if (subRange) {
                if (subRange.start.row <= startRow) {
                    break;
                } else if (subRange.isMultiLine()) {
                    row = subRange.end.row;
                } else if (startIndent == indent) {
                    break;
                }
            }
            endRow = row;
        }
        
        return new Range(startRow, startColumn, endRow, session.getLine(endRow).length);
    };
    this.getCommentRegionBlock = function(session, line, row) {
        var startColumn = line.search(/\s*$/);
        var maxRow = session.getLength();
        var startRow = row;
        
        var re = /^\s*(?:\/\*|\/\/|--)#?(end)?region\b/;
        var depth = 1;
        while (++row < maxRow) {
            line = session.getLine(row);
            var m = re.exec(line);
            if (!m) continue;
            if (m[1]) depth--;
            else depth++;

            if (!depth) break;
        }

        var endRow = row;
        if (endRow > startRow) {
            return new Range(startRow, startColumn, endRow, line.length);
        }
    };

}).call(FoldMode.prototype);

});

ace.define("ace/mode/batchfile",[], function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextMode = require("./text").Mode;
var BatchFileHighlightRules = require("./batchfile_highlight_rules").BatchFileHighlightRules;
var FoldMode = require("./folding/cstyle").FoldMode;

var Mode = function() {
    this.HighlightRules = BatchFileHighlightRules;
    this.foldingRules = new FoldMode();
    this.$behaviour = this.$defaultBehaviour;
};
oop.inherits(Mode, TextMode);

(function() {
    this.lineCommentStart = "::";
    this.blockComment = "";
    this.$id = "ace/mode/batchfile";
}).call(Mode.prototype);

exports.Mode = Mode;
});
                (function() {
                    ace.require(["ace/mode/batchfile"], function(m) {
                        if (typeof module == "object" && typeof exports == "object" && module) {
                            module.exports = m;
                        }
                    });
                })();
            