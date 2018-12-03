

function getSelectedText()
{
    if(window.getSelection)
    { 
        return window.getSelection().toString(); 
    }
    else if(document.selection && document.selection.createRange)
    { 
        return document.selection.createRange().text; 
    } 
    
    return ''; 
}

function getSelectedNode()
{
    var text = "focused node: ";
    
    //text += document.documentElement.focusNode.nodeName;
    
    return text;
    //return window.getSelection().focusNode.nodeName;
    //return document.activeElement.nodeName;
}

function stopCallout()
{
    document.documentElement.style.webkitTouchCallout = "none";
}

function displayAlert()
{
    alert("Hello world!")
}

/*
function getHTMLElementsAtPoint(x, y)
{
	var tags = ",";
	var e = document.elementFromPoint(x,y);
	
	while (e)
	{
		if(e.nodeName)
		{
			tags += e.nodeName;
            if (e.href)
            {
                tags += '(' + e.href + ')';
            }
            
            if (e.text)
            {
                tags += '(' + e.text + ')';
            }
            
            tags += ',';
		}
		
		e = e.parentNode;
	}
	
    document.documentElement.style.webkitTouchCallout = "none";
    //document.documentElement.style.webkitUserSelect = "none”;
    
	return tags;
}
*/

function getElementText(e)
{
    var tags = '';
    
    if (e.text)
    {
        tags += e.text;
    }
    
    var child = e.childNodes;
    var i, total = child.length;
    
    for (i = 0; i < total; i++)
    {
        tags += getElementText(child[i]);
    }

    return tags;
}

function getHTMLElementsAtPoint(x, y)
{
	var tags = ",";
	var e = document.elementFromPoint(x,y);
	
	while (e)
	{
		if(e.nodeName)
		{
            if (e.href)
            {
                tags += e.nodeName;
                tags += '(' + e.href + ')';
                break;
            }
		}
		
		e = e.parentNode;
	}
    
	return tags;
}

function getVideoSource(video)
{
    var value = "";
    
    if (video.src)
    {
        return video.src;
    }
    else
    {
        var child = video.childNodes;
        var i, total = child.length;
        
        for (i = 0; i < total; i++)
        {
            if (child[i].src)
            {
                return child[i].src;
            }
        }
    }
    
    return value;
}

function getImageAndVideo(x, y, w, h)
{
	var text = "";
	var data= document.getElementsByTagName('img');
    
	var i, total = data.length;
	
	for(i = 0; i < total; i++)
  	{
  	    var E = data[i];
  		var eClip = E.getBoundingClientRect();
  		
  		var eWidth = eClip.right - eClip.left;
  		var eHeight = eClip.bottom - eClip.top;
  		
  	    if (eClip.left >= x && eClip.top >= y && eClip.right <= (x + w) && eClip.bottom <= (y + h))
  		{
  			if (eWidth >= 50 && eHeight >= 50)
  			{
  				text += 'IMG,' + eClip.left + ',' + eClip.top + ',' + eWidth + ',' + eHeight + ',';
  				text += E.src + '\n';
  			}
      	}
  	}
  	
  	data = document.getElementsByTagName('video');
  	total = data.length;
  	
  	for (i = 0; i < total; i++)
  	{
  	    var E = data[i];
  		var eClip = E.getBoundingClientRect();
  		
  		var eWidth = eClip.right - eClip.left;
  		var eHeight = eClip.bottom - eClip.top;
  		var src = getVideoSource(E);
  		
		text += 'VIDEO,' + eClip.left + ',' + eClip.top + ',' + eWidth + ',' + eHeight + ',';
  		text += src + '\n';
  	}
  	
  	return text;
}


