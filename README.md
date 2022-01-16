# dns-relay

一个基于 libuv 实现的高并发 DNS 中继服务器，具有转发、缓存功能。

## 功能/特性

- DNS 报文解析
- DNS 报文转发
- 红黑树、LRU 组成的二级 DNS 缓存
- A、CNAME、SOA、MX、AAAA 等多种类型支持
- 异步的网络通信

## 参考

[Domain Name System - Wikipedia](https://en.wikipedia.org/wiki/Domain_Name_System)

[RFC 1035](https://datatracker.ietf.org/doc/html/rfc1035)

[libuv documentation](http://docs.libuv.org/en/v1.x/)

[基于 libuv 库的 UDP 收/发广播消息代码实现 - cnblogs](https://www.cnblogs.com/xiemingwang/pool/5576785.html)

[维基百科 - 红黑树](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)

[各种字符串 Hash 函数比较 - BYVoid](https://byvoid.com/zhs/blog/string-hash-compare/)

[Chinese-uvbook - GitHub](https://github.com/luohaha/Chinese-uvbook)

[Makiras/makiras_dns_refact](https://github.com/Makiras/makiras_dns_refact)

[Linyxus/YaDNS](https://github.com/Linyxus/YaDNS)
