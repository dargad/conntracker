#include "flows.h"

/* seqs stored in memory */

static GSequence *tcpv4flows;
static GSequence *udpv4flows;
static GSequence *icmpv4flows;
static GSequence *tcpv6flows;
static GSequence *udpv6flows;
static GSequence *icmpv6flows;

/* net helpers */

gchar *ipv4_str(struct in_addr *addr)
{
	gchar temp[INET_ADDRSTRLEN];

	memset(temp, 0, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, addr, temp, INET_ADDRSTRLEN);

	return g_strdup(temp);
}

gchar *ipv6_str(struct in6_addr *addr)
{
	gchar temp[INET6_ADDRSTRLEN];

	memset(temp, 0, INET6_ADDRSTRLEN);
	inet_ntop(AF_INET6, addr, temp, INET6_ADDRSTRLEN);

	return g_strdup(temp);
}

/* functions used to sort existing sequences based on its elements */

gint
cmp_ipv4base(struct ipv4base one, struct ipv4base two)
{
	if (one.src.s_addr < two.src.s_addr)
		return LESS;
	if (one.src.s_addr > two.src.s_addr)
		return MORE;

	if (one.src.s_addr == two.src.s_addr)
	{
		if (one.dst.s_addr < two.dst.s_addr)
			return LESS;
		if (one.dst.s_addr > two.dst.s_addr)
			return MORE;
	}

	return EQUAL;
}

gint
cmp_portbase(struct portbase one, struct portbase two)
{
	if (one.dst < two.dst)
		return LESS;
	if (one.dst > two.dst)
		return MORE;

	if (one.dst == two.dst)
	{
		if (one.src < two.src)
			return LESS;
		if (one.src > two.src)
			return MORE;
	}

	return EQUAL;
}

gint
cmp_icmpbase(struct icmpbase one, struct icmpbase two)
{
	if (one.type < two.type)
		return LESS;
	if (one.type > two.type)
		return MORE;

	if (one.type == two.type)
	{
		if (one.code < two.code)
			return LESS;
		if (one.code > two.code)
			return MORE;
	}

	return EQUAL;
}

gint
cmp_ipv6base(struct ipv6base one, struct ipv6base two)
{
	/* ipv6 sorting done through its string as its easier */
	int res = 0;
	gchar *str1, *str2;

	str1 = ipv6_str(&one.src);
	str2 = ipv6_str(&two.src);

	res = g_strcmp0(str1, str2);

	g_free(str1);
	g_free(str2);

	if (res < 0)
		return LESS;
	if (res > 0)
		return MORE;

	if (res == 0)
	{
		str1 = ipv6_str(&one.dst);
		str2 = ipv6_str(&two.dst);

		res = g_strcmp0(str1, str2);

		g_free(str1);
		g_free(str2);

		if (res < 0)
			return LESS;
		if (res > 0)
			return MORE;
	}

	return EQUAL;
}

/* call proper comparison/sorting functions based on given type */

gint cmp_tcpv4flow(struct tcpv4flow *one, struct tcpv4flow *two)
{
	int res;

	if ((res = cmp_ipv4base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_portbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

gint cmp_udpv4flow(struct udpv4flow *one, struct udpv4flow *two)
{
	int res;

	if ((res = cmp_ipv4base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_portbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

gint cmp_icmpv4flow(struct icmpv4flow *one, struct icmpv4flow *two)
{
	int res;

	if ((res = cmp_ipv4base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_icmpbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

gint cmp_tcpv6flow(struct tcpv6flow *one, struct tcpv6flow *two)
{
	int res;

	if ((res = cmp_ipv6base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_portbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

gint cmp_udpv6flow(struct udpv6flow *one, struct udpv6flow *two)
{
	int res;

	if ((res = cmp_ipv6base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_portbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

gint cmp_icmpv6flow(struct icmpv6flow *one, struct icmpv6flow *two)
{
	int res;

	if ((res = cmp_ipv6base(one->addrs, two->addrs)) != EQUAL)
		return res;
	if ((res = cmp_icmpbase(one->base, two->base)) != EQUAL)
		return res;

	if (one->foot.reply < two->foot.reply)
		return LESS;
	if (one->foot.reply > two->foot.reply)
		return MORE;

	return EQUAL;
}

/* compare two given flows (tcpv4, udpv4, icmpv4, tcpv6, udpv6 or icmpv6) */

gint cmp_tcpv4flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct tcpv4flow *one = (struct tcpv4flow*) ptr_one;
	struct tcpv4flow *two = (struct tcpv4flow*) ptr_two;

	return cmp_tcpv4flow(one, two);
}

gint cmp_udpv4flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct udpv4flow *one = (struct udpv4flow*) ptr_one;
	struct udpv4flow *two = (struct udpv4flow*) ptr_two;

	return cmp_udpv4flow(one, two);
}

gint cmp_icmpv4flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct icmpv4flow *one = (struct icmpv4flow*) ptr_one;
	struct icmpv4flow *two = (struct icmpv4flow*) ptr_two;

	return cmp_icmpv4flow(one, two);
}

gint cmp_tcpv6flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct tcpv6flow *one = (struct tcpv6flow*) ptr_one;
	struct tcpv6flow *two = (struct tcpv6flow*) ptr_two;

	return cmp_tcpv6flow(one, two);
}

gint cmp_udpv6flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct udpv6flow *one = (struct udpv6flow*) ptr_one;
	struct udpv6flow *two = (struct udpv6flow*) ptr_two;

	return cmp_udpv6flow(one, two);
}

gint cmp_icmpv6flows(gconstpointer ptr_one, gconstpointer ptr_two, gpointer data)
{
	struct icmpv6flow *one = (struct icmpv6flow*) ptr_one;
	struct icmpv6flow *two = (struct icmpv6flow*) ptr_two;

	return cmp_icmpv6flow(one, two);
}

/* add flows based on given type */

gint add_tcpv4flows(struct tcpv4flow *flow) {
	struct tcpv4flow *temp;
	GSequenceIter *found, *found2;

	temp = g_malloc0(sizeof(struct tcpv4flow));
	memcpy(temp, flow, sizeof(struct tcpv4flow));

	found = g_sequence_lookup(tcpv4flows, temp, cmp_tcpv4flows, NULL);

	if (found == NULL) {
		switch (temp->foot.reply) {
		case 0:
			temp->foot.reply = 1;
			found2 = g_sequence_lookup(tcpv4flows, temp, cmp_tcpv4flows, NULL);
			temp->foot.reply = 0;
			if (found2 == NULL) {
				g_sequence_insert_sorted(tcpv4flows, temp, cmp_tcpv4flows, NULL);
				goto inserted;
			}
			break;
		case 1:
			temp->foot.reply = 0;
			found2 = g_sequence_lookup(tcpv4flows, temp, cmp_tcpv4flows, NULL);
			temp->foot.reply = 1;
			if (found2 != NULL) {
				g_sequence_remove(found2);
				g_sequence_insert_sorted(tcpv4flows, temp, cmp_tcpv4flows, NULL);
				goto inserted;
			}
			break;
		}
	}

	g_free(temp);

	inserted: return 0;
}

addflows(udpv4flow);
addflows(icmpv4flow);
addflows(tcpv6flow);
addflows(udpv6flow);
addflows(icmpv6flow);

/* call addflows */

gint addtcpv4flow(struct in_addr s, struct in_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct tcpv4flow flow;

	memset(&flow, '0', sizeof(struct tcpv4flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.src = ps;
	flow.base.dst = pd;
	flow.foot.reply = r;

	add_tcpv4flows(&flow);

	return SUCCESS;
}

gint addudpv4flow(struct in_addr s, struct in_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct udpv4flow flow;
	memset(&flow, '0', sizeof(struct udpv4flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.src = ps;
	flow.base.dst = pd;
	flow.foot.reply = r;

	add_udpv4flows(&flow);

	return SUCCESS;
}

gint addicmpv4flow(struct in_addr s, struct in_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct icmpv4flow flow;
	memset(&flow, '0', sizeof(struct icmpv4flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.type = ps;
	flow.base.code = pd;
	flow.foot.reply = r;

	add_icmpv4flows(&flow);

	return SUCCESS;
}

gint addtcpv6flow(struct in6_addr s, struct in6_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct tcpv6flow flow;
	memset(&flow, '0', sizeof(struct tcpv6flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.src = ps;
	flow.base.dst = pd;
	flow.foot.reply = r;

	add_tcpv6flows(&flow);

	return SUCCESS;
}

gint addudpv6flow(struct in6_addr s, struct in6_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct udpv6flow flow;
	memset(&flow, '0', sizeof(struct udpv6flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.src = ps;
	flow.base.dst = pd;
	flow.foot.reply = r;

	add_udpv6flows(&flow);

	return SUCCESS;
}

gint addicmpv6flow(struct in6_addr s, struct in6_addr d, uint16_t ps, uint16_t pd, uint8_t r)
{
	struct icmpv6flow flow;
	memset(&flow, '0', sizeof(struct icmpv6flow));

	flow.addrs.src = s;
	flow.addrs.dst = d;
	flow.base.type = ps;
	flow.base.code = pd;
	flow.foot.reply = r;

	add_icmpv6flows(&flow);

	return SUCCESS;
}

/* display the flows */

out(tcpv4flow, ipv4, " TCPv4 [%12d] src = %s (port=%u) to dst = %s (port=%u)%s\n", times++,
		src, ntohs(flow->base.src), dst, ntohs(flow->base.dst),
		flow->foot.reply ? " (confirmed)" : "");

out(udpv4flow, ipv4, " UDPv4 [%12d] src = %s (port=%u) to dst = %s (port=%u)%s\n", times++,
		src, ntohs(flow->base.src), dst, ntohs(flow->base.dst),
		flow->foot.reply ? " (confirmed)" : "");

out(icmpv4flow, ipv4, "ICMPv4 [%12d] src = %s to dst = %s (type=%u | code=%u)%s\n", times++,
		src, dst, (uint8_t) ntohs(flow->base.type), (uint8_t) ntohs(flow->base.code),
		flow->foot.reply ? " (confirmed)" : "");

out(tcpv6flow, ipv6, " TCPv6 [%12d] src = %s (port=%u) to dst = %s (port=%u)%s\n", times++,
		src, ntohs(flow->base.src), dst, ntohs(flow->base.dst),
		flow->foot.reply ? " (confirmed)" : "");

out(udpv6flow, ipv6, " UDPv6 [%12d] src = %s (port=%u) to dst = %s (port=%u)%s\n", times++,
		src, ntohs(flow->base.src), dst, ntohs(flow->base.dst),
		flow->foot.reply ? " (confirmed)" : "");

out(icmpv6flow, ipv6, "ICMPv6 [%12d] src = %s to dst = %s (type=%u | code=%u)%s\n", times++,
		src, dst, (uint8_t) ntohs(flow->base.type), (uint8_t) ntohs(flow->base.code),
		flow->foot.reply ? " (confirmed)" : "");

void out_all(void)
{
	/* tell user through syslog what logfile will contain the data */
	out_logfile();

	/* dump internal data into the logfile */
	g_sequence_foreach(tcpv4flows, out_tcpv4flows, NULL);
	g_sequence_foreach(udpv4flows, out_udpv4flows, NULL);
	g_sequence_foreach(icmpv4flows, out_icmpv4flows, NULL);
	g_sequence_foreach(tcpv6flows, out_tcpv6flows, NULL);
	g_sequence_foreach(udpv6flows, out_udpv6flows, NULL);
	g_sequence_foreach(icmpv6flows, out_icmpv6flows, NULL);
}

/* allocate and de-allocate flows */

void cleanflow(gpointer data)
{
	g_free(data);
}

void alloc_flows(void)
{
	tcpv4flows = g_sequence_new(cleanflow);
	udpv4flows = g_sequence_new(cleanflow);
	icmpv4flows = g_sequence_new(cleanflow);
	tcpv6flows = g_sequence_new(cleanflow);
	udpv6flows = g_sequence_new(cleanflow);
	icmpv6flows = g_sequence_new(cleanflow);
}

void free_flows(void)
{
	g_sequence_free(tcpv4flows);
	g_sequence_free(udpv4flows);
	g_sequence_free(icmpv4flows);
	g_sequence_free(tcpv6flows);
	g_sequence_free(udpv6flows);
	g_sequence_free(icmpv6flows);
}