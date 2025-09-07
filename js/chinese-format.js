/// 中文段落格式化
function FormatZhParagraph(paragraph) {
    // 删除全角汉字之间的空格，正则也能匹配全角空格
    str = paragraph.innerHTML.trim().replace(/([^\x00-\xff])\s([^\x00-\xff])/g, '$1$2');
    // 段首加两个空格
    const indent = '　　';
    paragraph.innerHTML = indent + str;
}

/// 中文文章格式化
function FormatZhArticle(containerId) {
    const container = document.getElementById(containerId);
    if (container == null) {
        return;
    }
    const parList = container.getElementsByTagName('p');
    for (var i = 0; i < parList.length; ++i) {
        FormatZhParagraph(parList[i]);
        // console.log(`FormatZhParagraph ${i}`);
    }
}

// 格式化当前文章内容
FormatZhArticle('article-content');
