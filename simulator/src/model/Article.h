/*
 * Article.h
 *
 *  Created on: 19-10-2011
 *      Author: wojtas
 */

#ifndef ARTICLE_H_
#define ARTICLE_H_

class Article {
public:
	Article();
	virtual ~Article();
	bool isFood ();
	bool setFood (bool food);
private:
	bool food;
};

#endif /* ARTICLE_H_ */
