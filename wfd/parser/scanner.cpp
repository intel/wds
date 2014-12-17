/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */


#include "scanner.h"
#include "headerscanner.h"
#include "messagescanner.h"
#include "errorscanner.h"

#include "reply.h"

namespace wfd {

BaseLexer::~BaseLexer() {
}

int BaseLexer::yylex(Parser::semantic_type *lval) {
  yylval = lval;
  return( yylex() );
}

Scanner::Scanner(std::istream* in, Message*& message) {
  if (!message) {
    lexer_.reset(new HeaderScanner(in));
  } else if (message->is_reply()) {
    Reply* reply = static_cast<Reply*>(message);
    if (reply->response_code() == 303)
      lexer_.reset(new ErrorScanner(in));
    else
      lexer_.reset(new MessageScanner(in, true));
  }

  if (!lexer_)
    lexer_.reset(new MessageScanner(in));
}

Scanner::~Scanner() {
}

int Scanner::yylex(Parser::semantic_type *lval) {
  return lexer_->yylex(lval);
}

} /* namespace wfd */
