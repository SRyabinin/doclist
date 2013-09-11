$.widget("rs.listview", {
    options: {
        itemsOnPage: 30,
        docName: 'DocList',
        // Count of screens in view
        maxScreenInView:5
    },
    parameters: {
        
        items: [],
    },
    _create: function () {

        this.element
        .addClass("listview-container");
        this._refresh();
        $(window).scroll($.proxy(this._scrollChanged, this));
    },
    _scrollChanged: function () {
        var top = $(window).scrollTop();
        
        // if we still in view do nothing.
        if(!this._isViewNearOutOfScreen())
            return;
        
        // otherwise refresh view elements.

        var extremeIndexes = this._calculateViewExtremeIndexes(top);

        // Seek elements out of view.
        var outOfViewItems = jQuery.grep($('.listview-item', this.element), function(itemDiv, n){
            var index = $(itemDiv).attr('id');
            return index < extremeIndexes.first || index > extremeIndexes.last; 
        });
        var itemClicked = this._itemClicked;
        // remove elements which became invisible.
        $.each(outOfViewItems, function (str, div){
            $(div)
            .off("click", itemClicked)
            .remove();
        });

        //If no more elements in view.
        if($('.listview-item:first').length == 0)
        {
            var domFragment = document.createDocumentFragment();
            for(var id=extremeIndexes.first; id <= extremeIndexes.last; id++ )
            {
                domFragment.appendChild(this._createItem(id));
            }
            this.element.append(domFragment);
            this._refreshText(extremeIndexes.first, extremeIndexes.last);
        }
        else
        {
            // gets indexes of first and last items in view.
            var crtExtremeIndexes = {
                first : parseInt($($('.listview-item:first').get(0)).attr("id")),
                last : parseInt($($('.listview-item:last').get(0)).attr("id"))
            };

            if(extremeIndexes.first < crtExtremeIndexes.first)
            {
                // adds elements in the top of view
                var domFragment = document.createDocumentFragment();
                for(var id=extremeIndexes.first; id < crtExtremeIndexes.first; id++ )
                {
                    domFragment.appendChild(this._createItem(id));
                }
                this.element.prepend(domFragment);
                this._refreshText(extremeIndexes.first, crtExtremeIndexes.first-1);
            }
            if(extremeIndexes.last > crtExtremeIndexes.last)
            {
                // adds elements at the end of view
                domFragment = document.createDocumentFragment();
                for(var id=crtExtremeIndexes.last+1; id <=extremeIndexes.last; id++ )
                {
                    domFragment.appendChild(this._createItem(id));
                }
                this.element.append(domFragment);
                this._refreshText(crtExtremeIndexes.last+1, extremeIndexes.last);
             }
        }
    },
    // return true if view is near out of screen and content should be update.
    _isViewNearOutOfScreen: function ()
    {
        var firstDivInView = $('.listview-item:first', this.element);
        if(firstDivInView.length == 0)
            return true;
        var windowTop = $(window).scrollTop();
        var windowHeight = $(window).height();

        var screen = {top: windowTop, bottom : windowTop+windowHeight};
        var lastDivInView = $('.listview-item:last', this.element);
        var view = {
            top : firstDivInView.position().top,
            bottom : lastDivInView.position().top+lastDivInView.height()
        };

        // if the view near out of screen.
        return screen.top < (view.top + windowHeight*2/3) ||
               screen.bottom > (view.bottom- windowHeight*2/3);
    },
    // Calculates index of top and bottom items for changed scroll position.
    _calculateViewExtremeIndexes: function(scrollTop)
    {
        var extremeItems = {};
        var items = this.parameters.items;
        var documentHeight = $(document).height();
        // The approximate number of items on screen.
        var itemsOnScreen = items.length*$(window).height()/$(document).height();
        var view=
        {
            top: scrollTop-this.options.maxScreenInView*$(window).height()/2,
            bottom: scrollTop+$(window).height()+this.options.maxScreenInView*$(window).height()/2
        };
        // Calculate approximate index of item befor current view top. 
        var i =Math.floor(items.length*scrollTop/$(document).height()-2*itemsOnScreen);
        //quik search item with top less than scrollTop
        while(i > 0 && items[i].top>view.top){
            i -= 1000;
        }
        if(i<0){
            i=0;
        }

        // find first visible item in view.
        for (; (i < items.length) && items[i].top < view.top; i++);
        extremeItems.first = Math.min(i, items.length-1);

        // find last visible item in view.
        for (; (i < items.length) && items[i].top < view.bottom; i++);
        extremeItems.last = Math.min(i, items.length-1);
        if(extremeItems.last - extremeItems.first > 3000)
        {
            var brake = true;
        }
        return extremeItems;
    },
    _createItem: function (id) {
        var item = this.parameters.items[id];
        var div = document.createElement('div');
        $(div)
        .attr("id", item.index)
        .addClass("listview-item")
        .css({
            top:item.top,
            height:item.height
        })
        .on( "click", this._itemClicked);
        return div;
    },
    _itemClicked: function (item)
    {
        $.each($(".listview-item.focus"), function (str, e) {
            $(e).removeClass("focus");
        });
        $(this).addClass("focus");
    },
    _getTextMetrics: function (el) {

        var h = 0, w = 0;

        var div = document.createElement('div');
        document.body.appendChild(div);
        $(div).css({
            position: 'absolute',
            left: -1000,
            top: -1000,
            display: 'none'
        });

        var styles = ['font-size', 'font-style', 'font-weight', 'font-family', 'line-height', 'text-transform', 'letter-spacing'];
        $(styles).each(function () {
            var s = this.toString();
            $(div).css(s, $(el).css(s));
        });

        $(div).html('x');
        h = $(div).outerHeight();
        w = $(div).outerWidth();
        $(div).remove();

        var ret = {
            height: h,
            width: w
        };

        return ret;
    },
    _refreshText: function (first, last) {
        $.getJSON('isapi/Release/DocumentsProvider.dll',
            {
                repository:this.options.docName,
                first:first,
                last:last
            },
            function (data) {
                $.each(data, function (key, item) {
                    var div = $("#"+item.id);
                    if(div !== null)
                        $("#"+item.id).html(item.text.replace(/\\n/g, '<br/>'));
                });
            }
        );
    },
    _refresh: function () {
        var div = $(document.createElement('div'));
        div.addClass("listview-item");
        this.element.append(div);
        var textMetrics = this._getTextMetrics(div);
        // Ќет времени разбиратьс€, как вписать listview-item в container.
        var divWidth = div.outerWidth() - textMetrics.width*2;
        div.remove();
        var self = this;
        $.getJSON('Data/' + this.options.docName + '.json', function (data) {
            var items = [];
            var height = 0;
            $.each(data, function (key, val) {
                var top = height;
                $.each(val, function (str, length) {
                    var textLength = length * textMetrics.width;
                    height += Math.ceil(textLength / divWidth) * textMetrics.height;
                });
                items.push({ index: key, top: top, height: height - top });
            });
            self.element.height(height);
            self.parameters.items = items;
            self._scrollChanged();
        });
    } 
});